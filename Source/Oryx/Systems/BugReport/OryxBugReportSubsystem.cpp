#include "OryxBugReportSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/App.h"
#include "Engine/World.h"

bool UOryxBugReportSubsystem::SendBugReport(const FString& Description, const FString& Category)
{
	if (DiscordWebhookURL.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("OryxBugReport: no DiscordWebhookURL configured — report not sent."));
		return false;
	}
	if (Description.TrimStartAndEnd().IsEmpty())
	{
		return false;
	}

	// Compose the message: category tag + player text + auto context.
	FString Message;
	if (!Category.IsEmpty())
	{
		Message += FString::Printf(TEXT("**[%s]**\n"), *Category);
	}
	Message += Description;
	Message += TEXT("\n");
	Message += BuildContextString();

	// Discord caps content at 2000 chars — truncate defensively.
	if (Message.Len() > 1990)
	{
		Message = Message.Left(1987) + TEXT("...");
	}

	// Build {"content": "..."} via the JSON serializer so escaping is handled correctly.
	const TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetStringField(TEXT("content"), Message);

	FString Body;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
	FJsonSerializer::Serialize(Root, Writer);

	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(DiscordWebhookURL);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Body);
	Request->OnProcessRequestComplete().BindUObject(this, &UOryxBugReportSubsystem::HandleResponse);

	const bool bStarted = Request->ProcessRequest();
	if (!bStarted)
	{
		UE_LOG(LogTemp, Warning, TEXT("OryxBugReport: ProcessRequest failed to start."));
	}
	return bStarted;
}

FString UOryxBugReportSubsystem::BuildContextString() const
{
	FString BuildConfig;
#if UE_BUILD_SHIPPING
	BuildConfig = TEXT("Shipping");
#elif UE_BUILD_TEST
	BuildConfig = TEXT("Test");
#elif UE_BUILD_DEBUG
	BuildConfig = TEXT("Debug");
#else
	BuildConfig = TEXT("Development");
#endif

	FString MapName = TEXT("Unknown");
	if (const UGameInstance* GI = GetGameInstance())
	{
		if (const UWorld* World = GI->GetWorld())
		{
			MapName = World->GetMapName();
			MapName.RemoveFromStart(World->StreamingLevelsPrefix); // strip PIE "UEDPIE_0_" noise
		}
	}

	const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));
	const FString AppVersion = FApp::GetBuildVersion();

	return FString::Printf(
		TEXT("\n— — —\nMap: %s | Build: %s | Version: %s | %s"),
		*MapName, *BuildConfig, *AppVersion, *Timestamp);
}

void UOryxBugReportSubsystem::HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	bool bSuccess = false;
	FString Status;

	if (!bConnectedSuccessfully || !Response.IsValid())
	{
		Status = TEXT("Could not reach the server. Check your connection and try again.");
	}
	else
	{
		const int32 Code = Response->GetResponseCode();
		// Discord webhooks return 204 No Content on success.
		bSuccess = (Code >= 200 && Code < 300);
		Status = bSuccess
			? TEXT("Bug report sent — thank you!")
			: FString::Printf(TEXT("Report failed (HTTP %d). Please try again."), Code);
	}

	UE_LOG(LogTemp, Log, TEXT("OryxBugReport: %s"), *Status);
	OnBugReportComplete.Broadcast(bSuccess, Status);
}
