// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealPracticeProject/Public/HTTPComponent.h"
#include "HttpComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

//JSON 관련 헤더
#include "Json.h"

//FJsonObjectConverter 사용 시 필요한 헤더
#include "JsonUtilities.h"

UHTTPComponent::UHTTPComponent()
{
	//틱 비활성화
	PrimaryComponentTick.bCanEverTick = false;
}

void UHTTPComponent::SendGetRequest(const FString& Path)
{
	//HTTP 모듈 가져오기
	FHttpModule& HttpModule = FHttpModule::Get();

	//HTTP 요청 생성
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
    
	//HTTP GET RESPONSE가 왔을 때 실행할 콜백 메서드 바인딩
	Request->OnProcessRequestComplete().BindUObject(this, &UHTTPComponent::OnGetRequestComplete);

	//URL-GET-HEADER 요청 정보 설정
	Request->SetURL(ServerUrl + Path);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    
	//구축한 요청을 서버에 보내기
	Request->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("GET request sent to %s"), *Path);
}

void UHTTPComponent::SendPutRequest(const FString& Path)
{
	//HTTP 모듈 가져오기
	FHttpModule& HttpModule = FHttpModule::Get();
   
	//HTTP 요청 생성 
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
    
	//HTTP PUT RESPONSE가 왔을 때 실행할 콜백 메서드 바인딩
	Request->OnProcessRequestComplete().BindUObject(this, &UHTTPComponent::OnPutRequestComplete);

	//서버에 보낼 JSON 데이터 생성
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    
	//JSON의 문자열 필드
	JsonObject->SetStringField(TEXT("key"), TEXT("player_data"));
    
	//nested JSON 구조라면 객체를 따로 하나 만들어 삽입해야 한다 
	TSharedPtr<FJsonObject> ValueObject = MakeShareable(new FJsonObject());
	ValueObject->SetNumberField(TEXT("level"), 10);
	JsonObject->SetObjectField(TEXT("value"), ValueObject);

	//준비된 JSON 객체를 FString으로 변환
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	//URL-GET-HEADER 요청 정보 설정 및 본문에 JSON 데이터 담기
	Request->SetURL(ServerUrl + Path);
	Request->SetVerb(TEXT("PUT"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(JsonString);

	//구축한 요청을 서버에 보내기
	Request->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("PUT request sent to %s"), *Path);
}

void UHTTPComponent::OnGetRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//서버 응답이 성공했다면
	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		//서버로부터 받은 데이터를 FString으로 변환
		FString ResponseStr = Response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseStr);

		//FString을 FJsonObject로 파싱
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);

		//파싱에 성공했다면
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			//JsonObject에서 데이터 추출
			FString PlayerName = JsonObject->GetStringField(TEXT("name"));
			int32 PlayerLevel = JsonObject->GetIntegerField(TEXT("level"));
			
			//스크린에 출력
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red,
				FString::Printf(TEXT("Server Response : PlayerName %s, PlayerLevel %d"), *PlayerName, PlayerLevel));
			
			//추출한 데이터 출력
			UE_LOG(LogTemp, Warning, TEXT("Parsed Data -> Name: %s, Level: %d"), *PlayerName, PlayerLevel);
		}
	}
	else
	{
		//실패 사실 출력
		UE_LOG(LogTemp, Error, TEXT("GET request failed."));
	}
}

void UHTTPComponent::OnPutRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//서버 응답에 성공했다면
	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		//서버로부터 받은 데이터를 FString으로 변환
		const FString ResponseStr = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("PUT request successful! Server response: %s"), *ResponseStr);
	}
	else
	{
		//실패 사실 출력
		UE_LOG(LogTemp, Error, TEXT("PUT request failed."));
	}
}