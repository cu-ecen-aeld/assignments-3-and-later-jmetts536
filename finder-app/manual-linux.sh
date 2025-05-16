#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

# --------------------------------------------------------------------------------------------------------------------
# Create Output Directory OUTDIR
# --------------------------------------------------------------------------------------------------------------------

echo "Github Runner Test"
pwd
cp ~/arm-gnu-toolchains/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libm.so.6 ${OUTDIR}
exit 1

# Assign a value to OUTDIR based on provided argument if one provided, else use the default
if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

# Create the output directory OUTDIR
mkdir -p ${OUTDIR}

# Fail if the OUTDIR could not be created
if [ ! -d "${OUTDIR}" ]; then
	echo "FAILED TO CREATE DIRECTORY ${OUTDIR}."
	exit 1
fi

# --------------------------------------------------------------------------------------------------------------------
# Clone Linux-Stable Submodule
# --------------------------------------------------------------------------------------------------------------------

# Navigate to the output directory OUTDIR
cd "$OUTDIR"

# If the "/linux-stable" repository does not already exist in OUTDIR, then clone it in
if [ ! -d "${OUTDIR}/linux-stable" ]; then
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi

# --------------------------------------------------------------------------------------------------------------------
# Build Linux-Stable Image
# --------------------------------------------------------------------------------------------------------------------

# If the specified Image File does NOT Exist, then checkout the proper KERNEL_VERSION and build the Kernel
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    	cd linux-stable
    	echo "Checking out version ${KERNEL_VERSION}"
    	git checkout ${KERNEL_VERSION}

    	# TODO: Add your kernel build steps here

	# Clean the kernel build tree to prepare for a new build
	echo "Cleaning Build Tree"
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper

	# Build the default configuration defconfig (targets the 'virt' virtual arm dev board for QEMU)
	echo "Building Default Config"
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig

	# Build the VM Linux Target w/ multicore enable
	echo "Building Target ALL"
	make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all

	# Build any Kernel Modules
	echo "Building Target MODULES"
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules

	# Build the Devicetree
	echo "Building Target DTBS"
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs
fi

# If the Image directory already exists at the output directory OUTDIR, lets clean it to prepare for new Image
if [ -d "${OUTDIR}/Image" ]; then
	echo "Cleaning Image from the outdir"
	sudo rm -rf "${OUTDIR}/Image"
fi

# Recursively copy all Image files and Image Directory to the output directory OUTDIR
echo "Adding the Image in outdir"
cp "${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image" "${OUTDIR}/Image"

# --------------------------------------------------------------------------------------------------------------------
# Create /rootfs In Output Directory OUTDIR
# --------------------------------------------------------------------------------------------------------------------

# Navigate to the output directory OUTDIR
cd "$OUTDIR"

# If rootfs already exists in OUTDIR, remove it in preparation for the new rootfs
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    	sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories

# Create a rootfs directory in OUTDIR
echo "Creating the staging directory for the root filesystem"
mkdir -p "${OUTDIR}/rootfs"

# Fail if the /rootfs could not be created
if [ ! -d "${OUTDIR}/rootfs" ]; then
        echo "FAILED TO CREATE DIRECTORY ${OUTDIR}/rootfs."
        exit 1
fi

# Navigate to the rootfs directory
cd "${OUTDIR}/rootfs"

# Create the necessary base directories in the rootfs directory
echo "Populating rootfs with Base Directories"
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log

# --------------------------------------------------------------------------------------------------------------------
# Clone Busybox Submodule
# --------------------------------------------------------------------------------------------------------------------

# Navigate to the output directory OUTDIR
cd "$OUTDIR"

# If the OUTDIR does not already contain a /busybox directory, lets go ahead and create one
if [ ! -d "${OUTDIR}/busybox" ]; then
	echo "Cloning BusyBox Submodule into /busybox"
	git clone git://busybox.net/busybox.git
    	cd busybox
    	git checkout ${BUSYBOX_VERSION}
   
	# TODO:  Configure busybox
	
	# Do Stuff Here
else
	echo "Busybox Submodule Found"
    	cd busybox
fi

# --------------------------------------------------------------------------------------------------------------------
# Build Root File System with Busybox
# --------------------------------------------------------------------------------------------------------------------

# TODO: Make and install busybox

# Clean any previous build artifacts to prepare for new build
echo "Cleaning Busybox Build"
make distclean

# Build the default configuration defconfig
echo "Building Default Configuration for Busybox Build"
make defconfig

# Specify arcitecture and cross compiler
echo "Building Busybox with specified ARCH and CROSS_COMPILE"
make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}

# Specify path to rootfs
echo "Building Busybox with specified Root File System Path"
make CONFIG_PREFIX="${OUTDIR}/rootfs" ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

# --------------------------------------------------------------------------------------------------------------------
# Install Busybox Dependencies in Root File System
# --------------------------------------------------------------------------------------------------------------------

echo "Library dependencies"
cd "${OUTDIR}/rootfs"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs

# Copy the Program Interpreter to rootfs
echo "Copying Program Interpreter from ARM GNU Toolchain Folder to Root File System"
cp "/home/jmetts/arm-gnu-toolchains/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib/ld-linux-aarch64.so.1" "${OUTDIR}/rootfs/lib"

# Copy the libraries to rootfs
echo "Copying Libraries from ARM GNU Toolchain Folder to Root File System"
cp "/home/jmetts/arm-gnu-toolchains/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libm.so.6" "${OUTDIR}/rootfs/lib64"
cp "/home/jmetts/arm-gnu-toolchains/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libresolv.so.2" "${OUTDIR}/rootfs/lib64"
cp "/home/jmetts/arm-gnu-toolchains/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libc.so.6" "${OUTDIR}/rootfs/lib64"

# --------------------------------------------------------------------------------------------------------------------
# Create Device Nodes in the Root File System
# --------------------------------------------------------------------------------------------------------------------

# TODO: Make device nodes

# Create Null Device
echo "Creating Null Device in Root File System"
sudo mknod -m 666 ${OUTDIR}/rootfs/dev/null c 1 3

# Create Console Device
echo "Creating Console Device in Root File System"
sudo mknod -m 666 ${OUTDIR}/rootfs/dev/console c 5 1

# --------------------------------------------------------------------------------------------------------------------
# Build the Write Utility
# --------------------------------------------------------------------------------------------------------------------

# TODO: Clean and build the writer utility

# Navigate to the Finder-App Directory
cd "/home/jmetts/aesd-assignments/finder-app"

# Clean Any Previous Artifacts to Prepare for New Build
echo "Cleaning Writer Utility"
make clean

# Build the Writer Utility using the appropriate CROSS_COMPILEr
echo "Compiling Writer Utility"
make CROSS_COMPILE=${CROSS_COMPILE} all

# Copy the newly generated Writer Utility into Root File System /home
echo "Copying Writer Utility to Root File System"
cp "/home/jmetts/aesd-assignments/finder-app/writer" "${OUTDIR}/rootfs/home"

# Navigate to output directory OUTDIR
cd ${OUTDIR}

# --------------------------------------------------------------------------------------------------------------------
# Copy Executable Scripts of Interest to the Root File System
# --------------------------------------------------------------------------------------------------------------------

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs

# Copy Assignment Scripts/Files into Root File System /home
echo "Copying Assignment Scripts and Files to Root File System"
cp "/home/jmetts/aesd-assignments/finder-app/finder.sh" "${OUTDIR}/rootfs/home"
cp -r "/home/jmetts/aesd-assignments/conf" "${OUTDIR}/rootfs/home"
cp "/home/jmetts/aesd-assignments/finder-app/finder-test.sh" "${OUTDIR}/rootfs/home"
cp "/home/jmetts/aesd-assignments/finder-app/autorun-qemu.sh" "${OUTDIR}/rootfs/home"

# --------------------------------------------------------------------------------------------------------------------
# Change the Owner of the rootfs Directory
# --------------------------------------------------------------------------------------------------------------------

# TODO: Chown the root directory

# --------------------------------------------------------------------------------------------------------------------
# Create and Compress CPIO File for QEMU Emulation
# --------------------------------------------------------------------------------------------------------------------

# TODO: Create initramfs.cpio.gz

# Navigate to the rootfs Directory
cd "${OUTDIR}/rootfs"

# Create a CPIO file using all contents in the rootfs Directory
echo "Creating initramfs.cpio"
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio

# Navigate to the output directory OUTDIR
cd ${OUTDIR}

# Zip up the new CPIO file
echo "Compressing initramfs.cpio"
gzip -f initramfs.cpio
