/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdio>
#include <memory>
#include <string>

#include "mdf/ichannelobserver.h"
#include "mdf/mdffile.h"
#include "mdf/isamplereduction.h"

namespace mdf {

class IChannelGroup;

/** \brief Smart pointer to an observer. */
using ChannelObserverPtr = std::unique_ptr<IChannelObserver>;
/** \brief List of observer. */
using ChannelObserverList = std::vector<ChannelObserverPtr>;

/// \brief Returns true if the file is an MDF file.
[[nodiscard]] bool IsMdfFile(const std::string& filename);

/// \brief Creates and attaches a channel sample observer.
[[nodiscard]] ChannelObserverPtr CreateChannelObserver(
    const IDataGroup& data_group, const IChannelGroup& group,
    const IChannel& channel);
/**
 * Function that creates a channel observer by searching the channel name. In
 * case that the channel exist in many channel groups, it selects the group with
 * the largest number of samples.
 * @param dg_group The data group with the channel.
 * @param channel_name The channel name
 * @return A smart pointer to a channel observer.
 */
[[nodiscard]] ChannelObserverPtr CreateChannelObserver(
    const IDataGroup& dg_group, const std::string& channel_name);

/** \brief Creates a channel observer. */
void CreateChannelObserverForChannelGroup(const IDataGroup& data_group,
                                          const IChannelGroup& group,
                                          ChannelObserverList& dest);

/// \brief Creates and attaches a fast channel sample observer.
[[nodiscard]] ChannelObserverPtr CreateFastChannelObserver(
    const IDataGroup& data_group, const IChannelGroup& group,
    const IChannel& channel);
/**
 * Function that creates a fast channel observer by searching the channel name. In
 * case that the channel exist in many channel groups, it selects the group with
 * the largest number of samples.
 * @param dg_group The data group with the channel.
 * @param channel_name The channel name
 * @return A smart pointer to a channel observer.
 */
[[nodiscard]] ChannelObserverPtr CreateFastChannelObserver(
    const IDataGroup& dg_group, const std::string& channel_name);

/** \brief Creates a fast channel observer. */
void CreateFastChannelObserverForChannelGroup(const IDataGroup& data_group,
                                              const IChannelGroup& group,
                                              ChannelObserverList& dest);


/** \class MdfReader mdfreader.h "mdf/mdfreader.h"
 * \brief Reader interface to an MDF file.
 *
 * This is the main interface when reading MDF3 and MDF4 files.
 */
class MdfReader {
 public:
  explicit MdfReader(
      const std::string& filename);  ///< Constructor that opens the file and
                                     ///< read ID and HD block.
  virtual ~MdfReader();  ///< Destructor that close any open file and destructs.

  MdfReader() = delete;
  MdfReader(const MdfReader&) = delete;
  MdfReader(MdfReader&&) = delete;
  MdfReader& operator=(const MdfReader&) = delete;
  MdfReader& operator=(MdfReader&&) = delete;

  /**
   * Unique index for this reader. This index is typically used when fetching
   * files from a database. The index itself is not used by the reader.
   * @return An unique index.
   */
  [[nodiscard]] int64_t Index() const { return index_; }

  /**
   * Sets a unique index to this reader or actually its file. This index is
   * typically retrieved from a database and makes finding files much easier
   * than comparing paths.
   * @param index Unique index.
   */
  void Index(int64_t index) { index_ = index; }

  /// Checks if the file was read without errors.
  /// \return True if the file was read OK.
  [[nodiscard]] bool IsOk() const { return static_cast<bool>(instance_); }

  /// Returns a pointer to the MDF file. This file holds references to the MDF
  /// blocks. \return Pointer to the MDF file object. Note it may return a null
  /// pointer.
  [[nodiscard]] const MdfFile* GetFile() const { return instance_.get(); }

  /** \brief Returns the header (HD) block. */
  [[nodiscard]] const IHeader* GetHeader() const;
  /** \brief Returns the data group (DG) block. */
  [[nodiscard]] const IDataGroup* GetDataGroup(size_t order) const;

  [[nodiscard]] std::string ShortName()
      const;  ///< Returns the file name without paths.

  bool Open();   ///< Opens the file stream for reading.
  void Close();  ///< Closes the file stream.

  bool ReadHeader();             ///< Reads the ID and the HD block.
  bool ReadMeasurementInfo();    ///< Reads everything but not CG and raw data.
  bool ReadEverythingButData();  ///< Reads all blocks but not raw data.

  /** \brief Export the attachment data to a detination file. */
  bool ExportAttachmentData(const IAttachment& attachment,
                            const std::string& dest_file);

  /** \brief Reads all sample, sample reduction and signal data into memory.
   *
   * Reads in all data bytes that belongs to a data group (DG). The function
   * reads in sample data (DT..) blocks, sample reduction (RD/RV/RI) blocks
   * and signal data (SD) blocks. Note that this function may consume a lot
   * of memory, so remember to call the IDataGroup::ClearData() function
   * when data not are needed anymore.
   *
   * The attached observers also consumes memory, so remember to delete
   * them when they are no more needed.
   * @param data_group Reference to the data group (DG) object.
   * @return True if the red was successful.
   */
  bool ReadData(IDataGroup& data_group);
  /**
   * Reads selected sample from group , sample reduction and signal data into memory.
   * This function only read one frame data from group. When read big signal from channel,
   * read all data will cost more memory. Then you can use this function and only read you needed
   * signal data and only one frame data will be readed in memory. This will cost low memory.
   * @param data_group Reference to the data group (DG) object.
   * @param sample sample index, must less than total signal num.
   * @return True if the red was successful.
   */
  bool ReadOneData(IDataGroup& data_group);

  /** \brief Reads in data bytes to a sample reduction (SR) block.
   *
   * To minimíze the use of time and memory, this function reads in
   * data for one sample reduction (SR) block. The function is much
   * faster than to read in all data bytes for a data group (DG).
   * @param sr_group Reference to a sample reduction (SR) block.
   * @return True if the read was successful.
   */
  bool ReadSrData(ISampleReduction& sr_group);


 private:
  std::FILE* file_ = nullptr;          ///< Pointer to the file stream.
  std::string filename_;               ///< The file name with full path.
  std::unique_ptr<MdfFile> instance_;  ///< Pointer to the MDF file object.
  int64_t index_ = 0;  ///< Unique (database) file index that can be used to
                       ///< identify a file instead of its path.
};

}  // namespace mdf
