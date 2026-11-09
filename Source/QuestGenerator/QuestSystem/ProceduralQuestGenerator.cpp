// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralQuestGenerator.h"

void UProceduralQuestGenerator::Initialise(UQuestManager* Manager)
{
	QuestManager = Manager;
}

UQuest* UProceduralQuestGenerator::GenerateQuest()
{
	UQuest* Quest = NewObject<UQuest>(this);	
	Quest->QuestId = NextQuestID;
	NextQuestID++;
	
	
	return Quest;
}
