﻿//Copyright (c) 2023 Betide Studio. All Rights Reserved.


#include "EIK_GetPlayerData_AsyncFunction.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineUserCloudInterface.h"
#include "Kismet/GameplayStatics.h"

UEIK_GetPlayerData_AsyncFunction* UEIK_GetPlayerData_AsyncFunction::GetPlayerData(FString FileName)
{
	UEIK_GetPlayerData_AsyncFunction* BlueprintNode = NewObject<UEIK_GetPlayerData_AsyncFunction>();
	BlueprintNode->FileName = FileName;
	return BlueprintNode;
}

void UEIK_GetPlayerData_AsyncFunction::Activate()
{
	GetPlayerData();
	Super::Activate();
}

void UEIK_GetPlayerData_AsyncFunction::GetPlayerData()
{
	if(const IOnlineSubsystem *SubsystemRef = IOnlineSubsystem::Get())
	{
		if(const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface())
		{
			if(const IOnlineUserCloudPtr CloudPointerRef = SubsystemRef->GetUserCloudInterface())
			{
				TSharedPtr<const FUniqueNetId> UserIDRef = IdentityPointerRef->GetUniquePlayerId(0).ToSharedRef();
				CloudPointerRef->OnReadUserFileCompleteDelegates.AddUObject(this, &UEIK_GetPlayerData_AsyncFunction::OnGetFileComplete);
				CloudPointerRef->ReadUserFile(*UserIDRef, FileName);
			}
			else
			{
				if(!bDelegateCalled)
				{
					bDelegateCalled = true;
					OnFail.Broadcast(false, nullptr);
				}
			}
		}
		else
		{
			if(!bDelegateCalled)
			{
				bDelegateCalled = true;
				OnFail.Broadcast(false, nullptr);
			}
		}
	}
	else
	{
		if(!bDelegateCalled)
		{
			bDelegateCalled = true;
			OnFail.Broadcast(false, nullptr);
		}
	}
}

void UEIK_GetPlayerData_AsyncFunction::OnGetFileComplete(bool bSuccess, const FUniqueNetId& UserID,
	const FString& V_FileName)
{
	if(bSuccess)
	{
		if(const IOnlineSubsystem *SubsystemRef = IOnlineSubsystem::Get())
		{
			if(const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface())
			{
				if(const IOnlineUserCloudPtr CloudPointerRef = SubsystemRef->GetUserCloudInterface())
				{
					TSharedPtr<const FUniqueNetId> UserIDRef = IdentityPointerRef->GetUniquePlayerId(0).ToSharedRef();
					TArray<uint8> FileContents;
					CloudPointerRef->GetFileContents(*UserIDRef,FileName,FileContents);
					if(!FileContents.IsEmpty())
					{
						USaveGame* LocalSaveGame = UGameplayStatics::LoadGameFromMemory(FileContents);
						if(!bDelegateCalled)
						{
							bDelegateCalled = true;
							OnSuccess.Broadcast(true, LocalSaveGame);
						}
					}
					else
					{
						if(!bDelegateCalled)
						{
							bDelegateCalled = true;
							OnFail.Broadcast(false, nullptr);
						}
					}
				}
				else
				{
					if(!bDelegateCalled)
					{
						bDelegateCalled = true;
						OnFail.Broadcast(false, nullptr);
					}
				}
			}
			else
			{
				if(!bDelegateCalled)
				{
					bDelegateCalled = true;
					OnFail.Broadcast(false, nullptr);
				}
			}
		}
		else
		{
			if(!bDelegateCalled)
			{
				bDelegateCalled = true;
				OnFail.Broadcast(false, nullptr);
			}
		}
	}
	else
	{
		if(!bDelegateCalled)
		{
			bDelegateCalled = true;
			OnFail.Broadcast(false, nullptr);
		}
	}
}