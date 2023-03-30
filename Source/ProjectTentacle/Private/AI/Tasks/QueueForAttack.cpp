// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_QueueForAttack.h"

UBTTask_QueueForAttack::UBTTask_QueueForAttack()
{
	NodeName = "Queue for Attack";

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_QueueForAttack, BlackboardKey));
}