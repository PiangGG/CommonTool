// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpSubSystem.h"

#include "HttpModule.h"
#include "LoadingSubsystem.h"
#include "MessageSubsystem.h"
#include "CommonTool/Library/ToolFunctionLibrary.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

UHttpSubSystem* UHttpSubSystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		UHttpSubSystem* Router = UGameInstance::GetSubsystem<UHttpSubSystem>(GWorld->GetGameInstance());
		check(Router);
		return Router;
	}
	return nullptr;
}

void UHttpSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UToolFunctionLibrary::Debug(FString("HttpSubSystem::Initialize"));
}

void UHttpSubSystem::Deinitialize()
{
	UToolFunctionLibrary::Debug(FString("HttpSubSystem::Deinitialize"));
	Super::Deinitialize();
}

void UHttpSubSystem::HttpPost(const FString& Url, const FString& Data, const FHttpSingleCallBack& SingleCallBack,
                              int Retry, float TimeInterval)
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	if (Data.Len() > 0)
	{
		Request->SetContentAsString(Data);
	}
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	//Request->SetHeader(TEXT("Authorization"), Token);

	Request->OnProcessRequestComplete().BindLambda(
		[this,Url,Data,&SingleCallBack,Retry, TimeInterval](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
		                                                    bool bSucceeded)
		{
			bool isOk = false;
			if (bSucceeded && HttpResponse.IsValid())
			{
				int32 ResponseCode = HttpResponse->GetResponseCode();
				FString ResponseStr = HttpResponse->GetContentAsString();
				if (EHttpResponseCodes::IsOk(ResponseCode))
				{
					isOk = true;
					if (SingleCallBack.IsBound())
					{
						SingleCallBack.Execute(ResponseStr);
					}

					if (HttpCallBack.IsBound())
					{
						HttpCallBack.Broadcast(ResponseStr);
					}
				}
				else
				{
					FString ErrorStr = FString::Printf(
						TEXT("Invalid response. url: %s, code=%d error=%s"), *HttpRequest->GetURL(), ResponseCode,
						*ResponseStr);
#if !UE_BUILD_SHIPPING
					UToolFunctionLibrary::Error(ErrorStr);
#endif
				}
			}
			else
			{
				//网络问题，请检查网络连接
				if (IsValid(this))
				{
					OnHttpNetWorkError(HttpRequest ? HttpRequest->GetURL() : TEXT(""));
				}
			}

			if (!isOk)
			{
				//重试
				if (Retry > 0)
				{
					UToolFunctionLibrary::Error(
						FString::Printf(TEXT("正在重试 Retry：%d =========>>url: %s,  Data: %s"), Retry, *Url, *Data));
				}
				else
				{
					UToolFunctionLibrary::Error(
						FString::Printf(TEXT("重试失败！=========>>url: %s,  Data: %s"), *Url, *Data));
				}
			}
		}
	);
	Request->ProcessRequest();

	UToolFunctionLibrary::Debug(FString::Printf(TEXT("HttpPost正在请求:: =========>>url: %s,  Data: %s"), *Url, *Data));
}

void UHttpSubSystem::HttpPostCB(const FString& Url, const FString& Data, const FHttpSingleCallBack& SingleCallBack)
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	if (Data.Len() > 0)
	{
		Request->SetContentAsString(Data);
	}
	Request->SetVerb(TEXT("POST"));
	// Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	//Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ")+ Token);
	//Request->SetHeader(TEXT("X-Authorization"), TEXT("Bearer ")+ RefreshToken);

	Request->OnProcessRequestComplete().BindLambda(
		[this,SingleCallBack](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			if (bSucceeded && HttpResponse.IsValid())
			{
				int32 ResponseCode = HttpResponse->GetResponseCode();
				FString ResponseStr = HttpResponse->GetContentAsString();
				if (EHttpResponseCodes::IsOk(ResponseCode))
				{
					FString JsonStr = ResponseStr;
					TSharedPtr<FJsonObject> JsonObject;
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
					if (FJsonSerializer::Deserialize(Reader, JsonObject))
					{
						if (SingleCallBack.IsBound())
						{
							SingleCallBack.Execute(JsonStr);
						}

						if (HttpCallBack.IsBound())
						{
							HttpCallBack.Broadcast(ResponseStr);
						}
					}
				}
				else
				{
					FString ErrorStr = FString::Printf(
						TEXT("Invalid response. url: %s, code=%d error=%s"), *HttpRequest->GetURL(), ResponseCode,
						*ResponseStr);
					UToolFunctionLibrary::Error(ErrorStr);
				}
			}
			else
			{
			}
		});
	Request->ProcessRequest();
}

void UHttpSubSystem::HttpGetCB(const FString& Url, const FString& Data, const FHttpSingleCallBack& SingleCallBack)
{
	ULoadingSubsystem* LoadingSubsystem = ULoadingSubsystem::Get(this);
	if (LoadingSubsystem)
	{
		LoadingSubsystem->ShowLoadScreen(true);
	}
	
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	FString GetURL = FString(Url);
	if (Data.Len() > 0)
	{
		GetURL.Append("?").Append(Data);
		//Request->SetContentAsString(Data);
	}
	Request->SetURL(GetURL);
	//Request->SetVerb(TEXT("POST"));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	//Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ")+ Token);
	//Request->SetHeader(TEXT("X-Authorization"), TEXT("Bearer ")+ RefreshToken);
	
	FHttpSingleCallBack SingleCallBackState;
	SingleCallBackState.BindDynamic(this, &ThisClass::HttpGetCBComplete);
	
	Request->OnProcessRequestComplete().BindLambda(
		[this,SingleCallBack,SingleCallBackState](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			if (bSucceeded && HttpResponse.IsValid())
			{
				int32 ResponseCode = HttpResponse->GetResponseCode();
				FString ResponseStr = HttpResponse->GetContentAsString();
				if (EHttpResponseCodes::IsOk(ResponseCode))
				{
					FString JsonStr = ResponseStr;
					TSharedPtr<FJsonObject> JsonObject;
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
					if (FJsonSerializer::Deserialize(Reader, JsonObject))
					{
						if (SingleCallBack.IsBound())
						{
							SingleCallBack.Execute(JsonStr);
						}

						if (HttpCallBack.IsBound())
						{
							HttpCallBack.Broadcast(ResponseStr);
						}
					}
				}
				else
				{
					FString ErrorStr = FString::Printf(
							TEXT("Invalid response. url: %s, code=%d error=%s"), *HttpRequest->GetURL(), ResponseCode,
							*ResponseStr);
					UToolFunctionLibrary::Error(ErrorStr);
				}
			}
			else
			{
				FString ErrorStr = FString::Printf(
						TEXT("Invalid response. url: %s,"), *HttpRequest->GetURL());
					UToolFunctionLibrary::Error(ErrorStr);
			}
			if (SingleCallBackState.IsBound())
			{
				SingleCallBackState.Execute(TEXT(""));
			}
		});
	Request->ProcessRequest();
			
}

void UHttpSubSystem::HttpGetCBComplete(const FString& string)
{
	ULoadingSubsystem::Get(this)->ShowLoadScreen(false);
}

void UHttpSubSystem::HttpPostDeepSeek(const FString& Url, const FString& Token, const FString& Data,
                                      const FHttpSingleCallBack& SingleCallBack)
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	if (Data.Len() > 0)
	{
		Request->SetContentAsString(Data);
	}
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + Token);

	Request->OnProcessRequestComplete().BindLambda(
		[this,SingleCallBack](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			if (bSucceeded && HttpResponse.IsValid())
			{
				int32 ResponseCode = HttpResponse->GetResponseCode();
				FString ResponseStr = HttpResponse->GetContentAsString();
				if (EHttpResponseCodes::IsOk(ResponseCode))
				{
					FString JsonStr = ResponseStr;
					TSharedPtr<FJsonObject> JsonObject;
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
					if (FJsonSerializer::Deserialize(Reader, JsonObject))
					{
						if (SingleCallBack.IsBound())
						{
							SingleCallBack.Execute(JsonStr);
						}

						if (HttpCallBack.IsBound())
						{
							HttpCallBack.Broadcast(ResponseStr);
						}
					}
				}
				else
				{
					FString ErrorStr = FString::Printf(
						TEXT("Invalid response. url: %s, code=%d error=%s"), *HttpRequest->GetURL(), ResponseCode,
						*ResponseStr);
					UToolFunctionLibrary::Error(ErrorStr);
				}
			}
			else
			{
			}
		});
	Request->ProcessRequest();
}

void UHttpSubSystem::OnGameHttpError_Implementation(const FString& Url, const FString& Content)
{
}

void UHttpSubSystem::OnHttpNetWorkError_Implementation(const FString& Url)
{
	UToolFunctionLibrary::Error("访问错误:" + Url);
	HttpNetErrorHandle.Broadcast(Url);
}

void UHttpSubSystem::OnHttpError_Implementation(const FString& Url, const FString& Content)
{
}
