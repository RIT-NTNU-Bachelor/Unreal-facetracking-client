#include "MovableCamera.h"

// Sets default values
AMovableCamera::AMovableCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    HeadTrackingComponent = CreateDefaultSubobject<UHeadTracking>(TEXT("HeadTrackingComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;
    newLocation = FVector();
    
    PresetDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/Presets/CameraPresets.CameraPresets'")));
    if (PresetDataTable) LoadPresetsFromDataTable();
}

// Called when the game starts or when spawned
void AMovableCamera::BeginPlay()
{
	Super::BeginPlay();
    HeadTrackingComponent->StartHeadTracking();

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController(); // Retrieves the FP controller.
    // If FP controller is found, set new actor location and rotation.
    if (PlayerController)
    {
        APawn* CurrentPawn = PlayerController->GetPawn();

        // Check if the current pawn is not null and is of type AHeadTracking
        AMovableCamera* CameraPawn = Cast<AMovableCamera>(CurrentPawn);
        if (CameraPawn != nullptr)
        {
            // If it's already a HeadTrackingPawn, just move it to the standard location and rotation
            CameraPawn->SetActorLocation(StartLocation);
            CameraPawn->SetActorRotation(StartDirection);
        }
    }
}

// Called every frame
void AMovableCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    UpdatePosition();
}

void AMovableCamera::UpdatePosition()
{
    FVector LastKnownPosition = StartLocation;
    FRotator LastKnownRotation = StartDirection;
    float FOVmax = 120;
    float FOVmin = 50;
    float ZFov;
    float ZLimit = 250.0f;

    HeadTrackingComponent->UpdateHeadPosition(newLocation);

    // New position of the camera after handling as FVector, the standard format of coordinates.
    if (IncludeMovement)
    {
        X = newLocation.X * XMovementSensitivity;
        Y = newLocation.Y * YMovementSensitivity;
        Z = newLocation.Z * ZMovementSensitivity;
        if (Z < 0 && abs(Z) > ZLimit) Z = -ZLimit;
        
        LastKnownPosition = StartLocation + FVector(Z, X, Y);
        CameraComponent->SetRelativeLocation(LastKnownPosition); // Sets new position in the world.
        UE_LOG(LogTemp, Warning, TEXT("X: %f, Y: %f, Z: %f"), X, Y, Z);
    }
    
    // Option to remove rotation aspect of camera movement in UE.
    if (IncludeRotation)
    {
        // New position of the camera after handling as FRotator, the standard format of rotation.
        LastKnownRotation = StartDirection + FRotator(newLocation.Y * YRotationSensitivity, newLocation.X * XRotationSensitivity, newLocation.Z * ZRotationSensitivity);
        CameraComponent->SetWorldRotation(LastKnownRotation); // Sets new rotation relative to parent.
    }

    // Option to include or remove fov.
    if (HeadTrackingComponent->ZAxis && FOVEnabled)
    {
        ZFov = abs((-newLocation.Z / FOVSensitivity) + 80.0f);

        if (ZFov < FOVmin)
        {
            ZFov = FOVmin;
        }
        else if (ZFov > FOVmax)
        {
            ZFov = FOVmax;
        }
        CameraComponent->SetFieldOfView(ZFov);
        UE_LOG(LogTemp, Warning, TEXT("FOV: %f"), ZFov);
    }
}

void AMovableCamera::ChangeCameraSettings(int32 PresetIndex)
{
    if (CameraPresets.IsValidIndex(PresetIndex))
    {
        FCameraPreset Preset = CameraPresets[PresetIndex];

        // Set the camera properties based on the preset
        IncludeRotation = Preset.IncRot;
        IncludeMovement = Preset.IncMov;
        FOVEnabled = Preset.IncFov;

        XMovementSensitivity = Preset.XMoveSen;
        YMovementSensitivity = Preset.YMoveSen;
        ZMovementSensitivity = Preset.ZMoveSen;

        XRotationSensitivity = Preset.XRotSen;
        YRotationSensitivity = Preset.YRotSen;
        ZRotationSensitivity = Preset.ZRotSen;

        FOVSensitivity = Preset.FOVSen;
    }
}

void AMovableCamera::LoadPresetsFromDataTable()
{
    if (PresetDataTable)
    {
        static const FString ContextString(TEXT("Camera Preset Data Table"));

        // Iterate over each row in the data table
        for (auto& RowName : PresetDataTable->GetRowNames())
        {
            // Retrieve each row as an FCameraPreset struct
            FCameraPreset* Preset = PresetDataTable->FindRow<FCameraPreset>(RowName, ContextString, true);

            if (Preset)
            {
                // Add the struct to your CameraPresets array
                CameraPresets.Add(*Preset);
            }
        }
    }
}

