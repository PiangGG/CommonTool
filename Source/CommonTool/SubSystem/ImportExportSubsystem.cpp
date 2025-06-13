// Fill out your copyright notice in the Description page of Project Settings.


#include "ImportExportSubsystem.h"

#include "EngineUtils.h"
#include "LoadingSubsystem.h"
#include "SceneManagerSubsystem.h"
#include "CommonTool/Library/PrintToolLibrary.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "DesktopPlatformModule.h"
#include "Engine/GameViewportClient.h"
#include "IDesktopPlatform.h"
#include "Engine/Engine.h"
#include "Widgets/SWindow.h"
#elif PLATFORM_WINDOWS
#include "HAL/FileManager.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Microsoft/COMPointer.h"
#include "Misc/Paths.h"
#include <commdlg.h>
#include <shlobj.h>
#include <Winver.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

UImportExportSubsystem* UImportExportSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		UImportExportSubsystem* Router = UGameInstance::GetSubsystem<UImportExportSubsystem>(GWorld->GetGameInstance());
		check(Router);
		return Router;
	}
	return nullptr;
}

void UImportExportSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UPrintToolLibrary::Debug(FString("UImportExportSubsystem::Initialize"));
	
}

void UImportExportSubsystem::Deinitialize()
{
	UPrintToolLibrary::Debug(FString("UImportExportSubsystem::Deinitialize"));
	Super::Deinitialize();
}

void UImportExportSubsystem::ImportScene()
{
	ULoadingSubsystem::ShowLoadScreen(true);

	TArray<FString> OutFilenames;

	FString FileTypes = TEXT(
		"All Files (*.udatasmith;*.gltf;*.glb;)|*.udatasmith;*.gltf;*.glb|Datasmith files (*.udatasmith)|*.udatasmith|GL Transmission Format (*.gltf;*.glb)|*.gltf;*.glb");
	FString DefaultPath = "";

	bool bOpenFileDialog = false;
#if WITH_EDITOR
	if (GEngine && GEngine->GameViewport)
	{
		void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			//Opening the file picker!
			uint32 SelectionFlag = 0;
			//A value of 0 represents single file selection while a value of 1 represents multiple file selection
			bOpenFileDialog = DesktopPlatform->OpenFileDialog(ParentWindowHandle, TEXT("Choose A File"), DefaultPath, FString(""),
			                                FileTypes, SelectionFlag, OutFilenames);
		}
	}
#elif PLATFORM_WINDOWS
	TComPtr<IFileDialog> FileDialog;
	if (SUCCEEDED(::CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, IID_PPV_ARGS_Helper(&FileDialog))))
	{
		// Set up common settings
		FileDialog->SetTitle(TEXT("Choose A File"));
		if (!DefaultPath.IsEmpty())
		{
			// SHCreateItemFromParsingName requires the given path be absolute and use \ rather than / as our normalized paths do
			FString DefaultWindowsPath = FPaths::ConvertRelativePathToFull(DefaultPath);
			DefaultWindowsPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);

			TComPtr<IShellItem> DefaultPathItem;
			if (SUCCEEDED(::SHCreateItemFromParsingName(*DefaultWindowsPath, nullptr, IID_PPV_ARGS(&DefaultPathItem))))
			{
				FileDialog->SetFolder(DefaultPathItem);
			}
		}

		// Set-up the file type filters
		TArray<FString> UnformattedExtensions;
		TArray<COMDLG_FILTERSPEC> FileDialogFilters;
		{
			const FString DefaultFileTypes = TEXT("All Files (*.*)|*.*");
			DefaultFileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);

			if (UnformattedExtensions.Num() % 2 == 0)
			{
				FileDialogFilters.Reserve(UnformattedExtensions.Num() / 2);
				for (int32 ExtensionIndex = 0; ExtensionIndex < UnformattedExtensions.Num();)
				{
					COMDLG_FILTERSPEC& NewFilterSpec = FileDialogFilters[FileDialogFilters.AddDefaulted()];
					NewFilterSpec.pszName = *UnformattedExtensions[ExtensionIndex++];
					NewFilterSpec.pszSpec = *UnformattedExtensions[ExtensionIndex++];
				}
			}
		}
		FileDialog->SetFileTypes(FileDialogFilters.Num(), FileDialogFilters.GetData());

		// Show the picker
		if (SUCCEEDED(FileDialog->Show(NULL)))
		{
			int32 OutFilterIndex = 0;
			if (SUCCEEDED(FileDialog->GetFileTypeIndex((UINT*)&OutFilterIndex)))
			{
				OutFilterIndex -= 1; // GetFileTypeIndex returns a 1-based index
			}

			TFunction<void(const FString&)> AddOutFilename = [&OutFilenames](const FString& InFilename)
			{
				FString& OutFilename = OutFilenames.Add_GetRef(InFilename);
				OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);
				FPaths::NormalizeFilename(OutFilename);
			};

			{
				IFileOpenDialog* FileOpenDialog = static_cast<IFileOpenDialog*>(FileDialog.Get());

				TComPtr<IShellItemArray> Results;
				if (SUCCEEDED(FileOpenDialog->GetResults(&Results)))
				{
					DWORD NumResults = 0;
					Results->GetCount(&NumResults);
					for (DWORD ResultIndex = 0; ResultIndex < NumResults; ++ResultIndex)
					{
						TComPtr<IShellItem> Result;
						if (SUCCEEDED(Results->GetItemAt(ResultIndex, &Result)))
						{
							PWSTR pFilePath = nullptr;
							if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
							{
								AddOutFilename(pFilePath);
								::CoTaskMemFree(pFilePath);
							}
						}
					}
					bOpenFileDialog = true;
				}else
				{
					bOpenFileDialog = false;
				}
			}
		}
	}
#endif

	if (bOpenFileDialog == false)
	{
		ULoadingSubsystem::ShowLoadScreen(false);
		return;
	}
	
	if (OutFilenames.Num() > 0)
	{
		if (!OutFilenames[0].IsEmpty())
		{
			OnImportScene.Broadcast(OutFilenames[0],false,FString("udatasmith"));
		}
	}
}

void UImportExportSubsystem::ImportScenePointCloudScene()
{
	ULoadingSubsystem::ShowLoadScreen(true);

	TArray<FString> OutFilenames;

	FString FileTypes = TEXT(
		"All Files (*.las)|*.las;");
	FString DefaultPath = "";

	bool bOpenFileDialog = false;
#if WITH_EDITOR
	if (GEngine && GEngine->GameViewport)
	{
		void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			//Opening the file picker!
			uint32 SelectionFlag = 0;
			//A value of 0 represents single file selection while a value of 1 represents multiple file selection
			bOpenFileDialog = DesktopPlatform->OpenFileDialog(ParentWindowHandle, TEXT("Choose A File"), DefaultPath, FString(""),
			                                FileTypes, SelectionFlag, OutFilenames);
		}
	}
#elif PLATFORM_WINDOWS
	TComPtr<IFileDialog> FileDialog;
	if (SUCCEEDED(::CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, IID_PPV_ARGS_Helper(&FileDialog))))
	{
		// Set up common settings
		FileDialog->SetTitle(TEXT("Choose A File"));
		if (!DefaultPath.IsEmpty())
		{
			// SHCreateItemFromParsingName requires the given path be absolute and use \ rather than / as our normalized paths do
			FString DefaultWindowsPath = FPaths::ConvertRelativePathToFull(DefaultPath);
			DefaultWindowsPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);

			TComPtr<IShellItem> DefaultPathItem;
			if (SUCCEEDED(::SHCreateItemFromParsingName(*DefaultWindowsPath, nullptr, IID_PPV_ARGS(&DefaultPathItem))))
			{
				FileDialog->SetFolder(DefaultPathItem);
			}
		}

		// Set-up the file type filters
		TArray<FString> UnformattedExtensions;
		TArray<COMDLG_FILTERSPEC> FileDialogFilters;
		{
			const FString DefaultFileTypes = TEXT("All Files (*.*)|*.*");
			DefaultFileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);

			if (UnformattedExtensions.Num() % 2 == 0)
			{
				FileDialogFilters.Reserve(UnformattedExtensions.Num() / 2);
				for (int32 ExtensionIndex = 0; ExtensionIndex < UnformattedExtensions.Num();)
				{
					COMDLG_FILTERSPEC& NewFilterSpec = FileDialogFilters[FileDialogFilters.AddDefaulted()];
					NewFilterSpec.pszName = *UnformattedExtensions[ExtensionIndex++];
					NewFilterSpec.pszSpec = *UnformattedExtensions[ExtensionIndex++];
				}
			}
		}
		FileDialog->SetFileTypes(FileDialogFilters.Num(), FileDialogFilters.GetData());

		// Show the picker
		if (SUCCEEDED(FileDialog->Show(NULL)))
		{
			int32 OutFilterIndex = 0;
			if (SUCCEEDED(FileDialog->GetFileTypeIndex((UINT*)&OutFilterIndex)))
			{
				OutFilterIndex -= 1; // GetFileTypeIndex returns a 1-based index
			}

			TFunction<void(const FString&)> AddOutFilename = [&OutFilenames](const FString& InFilename)
			{
				FString& OutFilename = OutFilenames.Add_GetRef(InFilename);
				OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);
				FPaths::NormalizeFilename(OutFilename);
			};

			{
				IFileOpenDialog* FileOpenDialog = static_cast<IFileOpenDialog*>(FileDialog.Get());

				TComPtr<IShellItemArray> Results;
				if (SUCCEEDED(FileOpenDialog->GetResults(&Results)))
				{
					DWORD NumResults = 0;
					Results->GetCount(&NumResults);
					for (DWORD ResultIndex = 0; ResultIndex < NumResults; ++ResultIndex)
					{
						TComPtr<IShellItem> Result;
						if (SUCCEEDED(Results->GetItemAt(ResultIndex, &Result)))
						{
							PWSTR pFilePath = nullptr;
							if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
							{
								AddOutFilename(pFilePath);
								::CoTaskMemFree(pFilePath);
							}
						}
					}
					bOpenFileDialog = true;
				}else
				{
					bOpenFileDialog = false;
				}
			}
		}
	}
#endif

	if (bOpenFileDialog == false)
	{
		ULoadingSubsystem::ShowLoadScreen(false);
		return;
	}
	
	if (OutFilenames.Num() > 0)
	{
		if (!OutFilenames[0].IsEmpty())
		{
			OnImportScene.Broadcast(OutFilenames[0],false,FString("las"));
		}
	}
}

FString UImportExportSubsystem::SelectedPath() const
{
	FString MyResult  = "";
	ULoadingSubsystem::ShowLoadScreen(true);

	TArray<FString> OutFilenames;

	// FString FileTypes = TEXT(
	// 	"All Files (*.udatasmith;*.gltf;*.glb;)|*.udatasmith;*.gltf;*.glb|Datasmith files (*.udatasmith)|*.udatasmith|GL Transmission Format (*.gltf;*.glb)|*.gltf;*.glb| files (*.las)|*.las|");
	// FString FileTypes = TEXT("All Files");
	FString DefaultPath = "";

	bool bOpenFileDialog = false;
#if WITH_EDITOR
	if (GEngine && GEngine->GameViewport)
	{
		void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			//Opening the file picker!
			uint32 SelectionFlag = 0;
			//A value of 0 represents single file selection while a value of 1 represents multiple file selection
			bOpenFileDialog = DesktopPlatform->OpenFileDialog(ParentWindowHandle, TEXT("Choose A File"), DefaultPath, FString(""),
			                                TEXT(""), SelectionFlag, OutFilenames);
		}
	}
#elif PLATFORM_WINDOWS
	TComPtr<IFileDialog> FileDialog;
	if (SUCCEEDED(::CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, IID_PPV_ARGS_Helper(&FileDialog))))
	{
		// Set up common settings
		FileDialog->SetTitle(TEXT("Choose A File"));
		if (!DefaultPath.IsEmpty())
		{
			// SHCreateItemFromParsingName requires the given path be absolute and use \ rather than / as our normalized paths do
			FString DefaultWindowsPath = FPaths::ConvertRelativePathToFull(DefaultPath);
			DefaultWindowsPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);

			TComPtr<IShellItem> DefaultPathItem;
			if (SUCCEEDED(::SHCreateItemFromParsingName(*DefaultWindowsPath, nullptr, IID_PPV_ARGS(&DefaultPathItem))))
			{
				FileDialog->SetFolder(DefaultPathItem);
			}
		}

		// Set-up the file type filters
		TArray<FString> UnformattedExtensions;
		TArray<COMDLG_FILTERSPEC> FileDialogFilters;
		{
			const FString DefaultFileTypes = TEXT("All Files (*.*)|*.*");
			DefaultFileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);

			if (UnformattedExtensions.Num() % 2 == 0)
			{
				FileDialogFilters.Reserve(UnformattedExtensions.Num() / 2);
				for (int32 ExtensionIndex = 0; ExtensionIndex < UnformattedExtensions.Num();)
				{
					COMDLG_FILTERSPEC& NewFilterSpec = FileDialogFilters[FileDialogFilters.AddDefaulted()];
					NewFilterSpec.pszName = *UnformattedExtensions[ExtensionIndex++];
					NewFilterSpec.pszSpec = *UnformattedExtensions[ExtensionIndex++];
				}
			}
		}
		FileDialog->SetFileTypes(FileDialogFilters.Num(), FileDialogFilters.GetData());

		// Show the picker
		if (SUCCEEDED(FileDialog->Show(NULL)))
		{
			int32 OutFilterIndex = 0;
			if (SUCCEEDED(FileDialog->GetFileTypeIndex((UINT*)&OutFilterIndex)))
			{
				OutFilterIndex -= 1; // GetFileTypeIndex returns a 1-based index
			}

			TFunction<void(const FString&)> AddOutFilename = [&OutFilenames](const FString& InFilename)
			{
				FString& OutFilename = OutFilenames.Add_GetRef(InFilename);
				OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);
				FPaths::NormalizeFilename(OutFilename);
			};

			{
				IFileOpenDialog* FileOpenDialog = static_cast<IFileOpenDialog*>(FileDialog.Get());

				TComPtr<IShellItemArray> Results;
				if (SUCCEEDED(FileOpenDialog->GetResults(&Results)))
				{
					DWORD NumResults = 0;
					Results->GetCount(&NumResults);
					for (DWORD ResultIndex = 0; ResultIndex < NumResults; ++ResultIndex)
					{
						TComPtr<IShellItem> Result;
						if (SUCCEEDED(Results->GetItemAt(ResultIndex, &Result)))
						{
							PWSTR pFilePath = nullptr;
							if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
							{
								AddOutFilename(pFilePath);
								::CoTaskMemFree(pFilePath);
							}
						}
					}
					bOpenFileDialog = true;
				}else
				{
					bOpenFileDialog = false;
				}
			}
		}
	}
#endif

	if (bOpenFileDialog == false)
	{
		ULoadingSubsystem::ShowLoadScreen(false);
	}
	
	if (OutFilenames.Num() > 0)
	{
		if (!OutFilenames[0].IsEmpty())
		{
			MyResult = OutFilenames[0];
		}
	}
	ULoadingSubsystem::ShowLoadScreen(false);
	return MyResult;
}
