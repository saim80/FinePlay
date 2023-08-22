#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FineTravelPath.generated.h"

/// This class offers a character to travel between two points. These points are marked by sphere collision component.
///
/// For player characters, the characters enter one of the points and press the action button to travel to the other
///		point.
/// For AI characters, the characters enter one of the points and automatically travel to the other point.
/// For companion characters that follow player characters, the characters automatically travel between the two points
///		by following player characters.
///
/// The travel path is defined by two points. The points are marked by sphere collision component.
///
/// There are three travel modes:
/// - Interpolate: Using timer, the character is moved from one point to the other point by periodic sweep relocation.
/// - Teleport: The character is teleported from one point to the other point.
/// - Navigation: Using navigation mesh, the character is moved from one point to the other point.
/// - Controller: Using controller's AddMovementInput, the character is moved from one point to the other point.
///
/// When the characters are traveling, no user input is accepted. The characters become invincible.
///
/// In addition, one additional sphere collision component per point can be added to define the entrance.
/// When the characters enter the entrance, the characters are moved to the nearest point to move to the farthest point.
/// After completing the travel, the characters are moved to the closest entrance.
///
/// The movement between the travel point and entrance is always done by Controller mode.
///
/// After the trip, the character is released from invincibility and input is accepted for player characters.
UCLASS()
class FINEPLAY_API AFineTravelPath : public AActor
{
	GENERATED_BODY()

public:
	AFineTravelPath();
};
