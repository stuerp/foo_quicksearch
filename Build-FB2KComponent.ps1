<#
.SYNOPSIS
    Builds the foobar2000 component package.
.DESCRIPTION
    This script will be executed unconditionally during the Post-build step. It copies all the necessary files to an output directory and creates the zip archive.
.EXAMPLE
    C:\PS> .\Build-FB2KComponent.ps1
.OUTPUTS
    foo_midi.fb2k-component
#>

[CmdletBinding()]
param
(
    [parameter(Mandatory, HelpMessage='Target Name')]
        [string] $TargetName,
    [parameter(Mandatory, HelpMessage='Target File Name')]
        [string] $TargetFileName,
    [parameter(Mandatory, HelpMessage='Platform')]
        [string] $Platform,
    [parameter(Mandatory, HelpMessage='OutputPath')]
        [string] $OutputPath
)

#Requires -Version 7.2

Set-StrictMode -Version Latest;
Set-PSDebug -Strict; # Equivalent of VBA "Option Explicit".

$ErrorActionPreference = 'Stop';

$PSStyle.OutputRendering = [System.Management.Automation.OutputRendering]::PlainText;

# Note: The working directory is the solution directory.

Write-Host "Building package `"$TargetName.fb2k-component`"...";

if ($Platform -eq 'x64')
{
    $TargetPath = "$TargetName/x64";
    $PackagePath = "../out/$TargetPath";

    Write-Host "Creating output directory `"$PackagePath`"...";
    $null = New-Item -Path '../out/' -Name $TargetPath -ItemType 'directory' -Force;

    if (Test-Path -Path "$OutputPath/$TargetFileName")
    {
        Write-Host "Copying `"$TargetFileName`" to `"$PackagePath`"...";
        Copy-Item "$OutputPath/$TargetFileName" -Destination "$PackagePath";
    }

    # install the component in the foobar2000 x64 components directory.
    Write-Host "Installing component in foobar2000 64-bit...";

    $foobar2000Path = '../bin';

    if (Test-Path -Path "$foobar2000Path/foobar2000.exe")
    {
        $ComponentPath = "$foobar2000Path/profile/user-components-x64/$TargetName";

        Write-Host "Creating directory `"$ComponentPath`"...";
        $null = New-Item -Path "$foobar2000Path/profile/user-components-x64/" -Name "$TargetName" -ItemType 'directory' -Force;

        Write-Host "Copying component files...";
        Copy-Item "$PackagePath/*" $ComponentPath -Force;
    }

}
elseif ($Platform -eq 'Win32')
{
    $TargetPath = "$TargetName";
    $PackagePath = "../out/$TargetPath";

    Write-Host "Creating output directory `"$PackagePath`"...";
    $null = New-Item -Path '../out/' -Name $TargetPath -ItemType 'directory' -Force;

    if (Test-Path -Path "$OutputPath/$TargetFileName")
    {
        Write-Host "Copying `"$TargetFileName`" to `"$PackagePath`"...";
        Copy-Item "$OutputPath/$TargetFileName" -Destination "$PackagePath" -Force;
    }

    # install the component in the foobar2000 x86 components directory.
    Write-Host "Installing component in foobar2000 32-bit...";

    $foobar2000Path = '../bin/x86';

    if (Test-Path -Path "$foobar2000Path/foobar2000.exe")
    {
        $ComponentPath = "$foobar2000Path/profile/user-components/$TargetName";

        Write-Host "Creating directory `"$ComponentPath`"...";
        $null = New-Item -Path "$foobar2000Path/profile/user-components/" -Name "$TargetName" -ItemType 'directory' -Force;

        Write-Host "Copying component files...";
        Copy-Item "$PackagePath/*" $ComponentPath -Force;
    }
}
else
{
    Write-Host "Unknown platform: $Platform";
    exit;
}

Write-Host "Compressing archive `"../out/$TargetFileName.fb2k-component`"...";

Compress-Archive -Path ../out/$TargetName/* -DestinationPath ../out/$TargetName.fb2k-component -Force;

Write-Host "Done.";
