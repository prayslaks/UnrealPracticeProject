// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//HTTP 모듈 헤더
#include "HttpModule.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HTTPComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREALPRACTICE_API UHTTPComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHTTPComponent();

	//블루프린트에서 호출할 서버 GET 요청 함수
	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void SendGetRequest(const FString& Path);

	//블루프린트에서 호출할 서버 PUT 요청 함수
	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void SendPutRequest(const FString& Path);

private:
	//GET 요청이 완료되었을 때 호출될 콜백 함수
	void OnGetRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//PUT 요청이 완료되었을 때 호출될 콜백 함수
	void OnPutRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    
	//서버 주소 저장
	FString ServerUrl = TEXT("http://127.0.0.1:5000");
};