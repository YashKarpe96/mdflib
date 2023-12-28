/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <memory>
#include <string>
#include <vector>

#include "cg4block.h"
#include "datalistblock.h"
#include "mdf/idatagroup.h"

namespace mdf::detail {
class Dg4Block : public DataListBlock, public IDataGroup {
 public:
  using Cg4List = std::vector<std::unique_ptr<Cg4Block>>;
  Dg4Block();

  [[nodiscard]] int64_t Index() const override;
  [[nodiscard]] std::string BlockType() const override {
    return MdfBlock::BlockType();
  }
  void Description(const std::string& desc) override;
  [[nodiscard]] std::string Description() const override;

  [[nodiscard]] std::vector<IChannelGroup*> ChannelGroups() const override;
  [[nodiscard]] IChannelGroup* CreateChannelGroup() override;

  void AddCg4(std::unique_ptr<Cg4Block>& cg4);
  [[nodiscard]] const Cg4List& Cg4() const { return cg_list_; }

  void GetBlockProperty(BlockPropertyList& dest) const override;
  MdfBlock* Find(int64_t index) const override;

  size_t Read(std::FILE* file) override;
  void ReadCgList(std::FILE* file);

  void ReadData(std::FILE* file);
  bool ReadOneData(std::FILE* file);
  void ClearData() override;

  IMetaData* CreateMetaData() override;
  IMetaData* MetaData() const override;
  void RecordIdSize(uint8_t id_size) override;
  uint8_t RecordIdSize() const override;
  size_t Write(std::FILE* file) override;
  size_t DataSize() const override;

  bool UpdateDtBlocks(std::FILE *file);
  bool UpdateCgAndVlsdBlocks(std::FILE *file, bool update_cg, bool update_vlsd);

  [[nodiscard]] IChannelGroup *FindParentChannelGroup(
      const IChannel &channel) const override;
  [[nodiscard]] Cg4Block* FindCgRecordId(uint64_t record_id) const;
 private:
  struct read_one_data_info_t{
    size_t read_count = 0;
    size_t total_data_size = 0;
    bool valid = false;
    FILE* data_file = nullptr;
    bool close_file = false;
  };
  read_one_data_info_t one_data_info_;
  uint8_t rec_id_size_ = 0;
  /* 7 byte reserved */
  Cg4List cg_list_;

  void ParseDataRecords(std::FILE* file, size_t nof_data_bytes);
  bool ParseDataRecordsEx(std::FILE* file, size_t& current_count, size_t nof_data_bytes);
  size_t ReadRecordId(std::FILE* file, uint64_t& record_id) const;

};

}  // namespace mdf::detail
