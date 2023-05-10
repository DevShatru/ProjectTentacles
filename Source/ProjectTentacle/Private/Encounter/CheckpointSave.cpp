// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/CheckpointSave.h"

FString UCheckpointSave::GetSlotName()
{
	return SlotName;
}

int32 UCheckpointSave::GetSlotIndex() const
{
	return SlotIndex;
}
