// Fill out your copyright notice in the Description page of Project Settings.


#include "NameGenerator.h"

UNameGenerator::UNameGenerator()
{
    Prefixes =
    {
        TEXT("Raven"),
        TEXT("Red"),
        TEXT("White"),
        TEXT("Black"),
        TEXT("Silver"),
        TEXT("Grey"),
        TEXT("Golden"),
        TEXT("Dark"),
        TEXT("Dragon"),
        TEXT("Blood"),
        TEXT("Shade"),
        TEXT("Sun's "),
        TEXT("Dawn"),
        TEXT("Stone"),
        TEXT("Storm's "),
        TEXT("Iron"),
        TEXT("Star")
    };

    Suffixes.Add(ELocationType::NONE,
    {
        TEXT("rest"),
        TEXT("hold"),
        TEXT("haven"),
        TEXT("ton"),
        TEXT("vale")
    });

    Suffixes.Add(ELocationType::Forest,
    {
        TEXT("wood"),
        TEXT("lodge"),
        TEXT("Oak"),
        TEXT("wold")
    });

    Suffixes.Add(ELocationType::River,
    {
        TEXT("crossing"),
        TEXT("bridge"),
        TEXT("ford"),
        TEXT("well"),
        TEXT("brook")
    });

    Suffixes.Add(ELocationType::Mountain,
    {
        TEXT("pass"),
        TEXT("ridge"),
        TEXT("vale"),
        TEXT("don")
    });

    Suffixes.Add(ELocationType::Swamp,
    {
        TEXT("moor"),
        TEXT("fen"),
        TEXT("slough"),
        TEXT("water"),
        TEXT("mire")
    });

    Suffixes.Add(ELocationType::Beach,
    {
        TEXT("shore"),
        TEXT("tide"),
        TEXT("smouth"),
        TEXT("harbour"),
        TEXT("port")
    });
}

FText UNameGenerator::GenerateName(ELocationType Biome)
{
    // select a random prefix
    FString Prefix = Prefixes[FMath::RandRange(0, Prefixes.Num() - 1)];

    // get pool of suffixes from location and generic
    TArray<FString> AvailableSuffixes = Suffixes[Biome];
    AvailableSuffixes.Append(Suffixes[ELocationType::NONE]);

    // select a random suffix from the pool
    FString Suffix = AvailableSuffixes[FMath::RandRange(0, AvailableSuffixes.Num() - 1)];
    
    // combine prefix and suffix and return
    return FText::FromString(Prefix + Suffix);
}