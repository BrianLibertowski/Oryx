#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "OryxBugReportSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBugReportComplete, bool, bSuccess, const FString&, Message);

/**
 *  In-game bug reporting via a Discord webhook. Players type a description on the menu's
 *  "Report a Bug" screen → SendBugReport posts it to a private Discord channel.
 *
 *  SETUP (one-time, before builds):
 *    1. Discord server → channel → Settings → Integrations → Webhooks → New Webhook → Copy URL.
 *    2. Paste it into Config/DefaultGame.ini:
 *         [/Script/Oryx.OryxBugReportSubsystem]
 *         DiscordWebhookURL=https://discord.com/api/webhooks/XXXX/YYYY
 *       (Config keeps the URL out of Blueprints. NOTE: it still ships inside the packaged build and
 *       can be extracted — fine for a CLOSED playtest; rotate the webhook after the test window.
 *       For a public release, route through a tiny server-side relay instead of a raw webhook.)
 *
 *  USAGE (BP, on the report screen's Submit button):
 *    Get Game Instance Subsystem (OryxBugReportSubsystem) → Send Bug Report (description text).
 *    Bind OnBugReportComplete to show a "Thanks!" / "Failed, try again" toast.
 *
 *  The posted message auto-appends context: build config, current map, timestamp, app version.
 */
UCLASS(Config = Game)
class ORYX_API UOryxBugReportSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Fires after the HTTP round-trip. bSuccess true on 2xx. Message = human-readable status for UI. */
	UPROPERTY(BlueprintAssignable, Category = "Oryx|BugReport")
	FOnBugReportComplete OnBugReportComplete;

	/**
	 *  Posts a bug report. Description is the player's free text. Category is an optional tag
	 *  ("Crash", "Visual", "Balance", …) the UI can pass from a dropdown — empty is fine.
	 *  Returns false immediately (without firing the delegate) if no webhook URL is configured
	 *  or the description is blank.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|BugReport")
	bool SendBugReport(const FString& Description, const FString& Category);

	/** True if a webhook URL is configured — UI can disable/hide the report button when false. */
	UFUNCTION(BlueprintPure, Category = "Oryx|BugReport")
	bool IsConfigured() const { return !DiscordWebhookURL.IsEmpty(); }

private:
	/** Discord webhook endpoint. Set in DefaultGame.ini — see class docs. Never hardcode in BP. */
	UPROPERTY(Config)
	FString DiscordWebhookURL;

	/** Builds the auto-appended context block (build config / map / version / timestamp). */
	FString BuildContextString() const;

	/** HTTP completion handler — translates status code into the OnBugReportComplete broadcast. */
	void HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};
