// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UFlecsQuickstartTarget : TargetRules
{
	public UFlecsQuickstartTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "UFlecsQuickstart", "FlecsLibrary", "UnrealFlecs", "MainGameplay"} );
	}
}
