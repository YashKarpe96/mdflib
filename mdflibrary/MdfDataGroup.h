﻿/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <mdf/idatagroup.h>
#include "MdfChannelGroup.h"
#include "MdfMetaData.h"

using namespace System;
namespace MdfLibrary {
public ref class MdfDataGroup {
public:
  property String^ Description { String^ get(); void set(String^ desc); }
  property uint8_t RecordIdSize {uint8_t get(); void set(uint8_t id_size); }
  property array<MdfChannelGroup^>^ ChannelGroups {
    array<MdfChannelGroup^>^ get();
  }
  property MdfMetaData^ MetaData {MdfMetaData^ get(); }

  MdfChannelGroup^ CreateChannelGroup();
  MdfMetaData^ CreateMetaData();
  bool IsRead();
  MdfChannelGroup^ FindParentChannelGroup(MdfChannel^ channel);
  void ResetSample();
  
private:
  MdfDataGroup() {};
internal:
  mdf::IDataGroup *group_ = nullptr; 
  MdfDataGroup(mdf::IDataGroup* group); 
};
}