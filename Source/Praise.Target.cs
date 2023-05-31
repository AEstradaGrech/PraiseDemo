// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class PraiseTarget : TargetRules
{
	public PraiseTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		bForceEnableExceptions = true;
		BuildEnvironment = TargetBuildEnvironment.Unique;
		ExtraModuleNames.AddRange( new string[] { "Praise" } );
	}
}
