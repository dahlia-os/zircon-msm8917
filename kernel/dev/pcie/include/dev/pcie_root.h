// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2016, Google, Inc. All rights reserved
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef ZIRCON_KERNEL_DEV_PCIE_INCLUDE_DEV_PCIE_ROOT_H_
#define ZIRCON_KERNEL_DEV_PCIE_INCLUDE_DEV_PCIE_ROOT_H_

#include <trace.h>
#include <zircon/compiler.h>

#include <dev/pcie_bus_driver.h>
#include <dev/pcie_ref_counted.h>
#include <dev/pcie_upstream_node.h>
#include <fbl/intrusive_wavl_tree.h>
#include <fbl/macros.h>
#include <fbl/ref_ptr.h>

class PcieRoot : public fbl::WAVLTreeContainable<fbl::RefPtr<PcieRoot>>, public PcieUpstreamNode {
 public:
  // Disallow copying, assigning and moving.
  DISALLOW_COPY_ASSIGN_AND_MOVE(PcieRoot);

  // Implement ref counting, do not let derived classes override.
  PCIE_IMPLEMENT_REFCOUNTED;

  // PCIE roots are already at the top of the topology so there
  // is no action necessary.
  zx_status_t EnableBusMasterUpstream(bool) override {
    TRACE_ENTRY;
    return ZX_OK;
  }
  // Properties
  PcieBusDriver& driver() { return bus_drv_; }
  RegionAllocator& pf_mmio_regions() final { return bus_drv_.pf_mmio_regions(); }
  RegionAllocator& mmio_lo_regions() final { return bus_drv_.mmio_lo_regions(); }
  RegionAllocator& mmio_hi_regions() final { return bus_drv_.mmio_hi_regions(); }
  RegionAllocator& pio_regions() final { return bus_drv_.pio_regions(); }

  // Perform the swizzle for the root which this swizzle interface applies to.
  //
  // When legacy IRQs traverse PCI/PCIe roots, they are subject to a platform
  // specific IRQ swizzle operation.  Platforms must supply an implementation
  // of this method for when they add a root to the bus driver before startup.
  //
  // @param dev_id  The device ID of the pcie device/bridge to swizzle for.
  // @param func_id The function ID of the pcie device/bridge to swizzle for.
  // @param pin     The pin we want to swizzle
  // @param irq     An output pointer for what IRQ this pin goes to
  //
  // @return ZX_OK if we successfully swizzled
  // @return ZX_ERR_NOT_FOUND if we did not know how to swizzle this pin
  virtual zx_status_t Swizzle(uint dev_id, uint func_id, uint pin, uint* irq) = 0;

  // WAVL-tree Index
  uint GetKey() const { return managed_bus_id(); }
  // TODO(johngro) : Add support for RCRB (root complex register block)  Consider splitting
  // PcieRoot into PciRoot and PcieRoot (since PciRoots don't have RCRBs)

 protected:
  PcieRoot(PcieBusDriver& bus_drv, uint mbus_id);

 private:
  PcieBusDriver& bus_drv_;
};

#endif  // ZIRCON_KERNEL_DEV_PCIE_INCLUDE_DEV_PCIE_ROOT_H_
