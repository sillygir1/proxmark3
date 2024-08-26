export DUO_BUILDROOT_SDK=/home/sillygir1/coding/duo-buildroot-sdk
export DUO_SDK=/home/sillygir1/coding/duo-sdk
export PILK_SOFTWARE=/home/sillygir1/coding/pilk-v-software
export LZ4_PATH=$DUO_BUILDROOT_SDK/buildroot-2021.05/output/milkv-duo256m_musl_riscv64/build/lz4-1.9.3/lib/
export BZIP2_PATH=$DUO_BUILDROOT_SDK/buildroot-2021.05/output/milkv-duo256m_musl_riscv64/build/bzip2-1.0.8/

make clean && CFLAGS="-L$LZ4_PATH -I$LZ4_PATH -L$BZIP2_PATH -I$BZIP2_PATH -I$DUO_SDK/rootfs/usr/include -I$PILK_SOFTWARE/lvgl/ -I$PILK_SOFTWARE/lvgl-launcher/src/ -I$PILK_SOFTWARE/duo-battery -I$PILK_SOFTWARE/duo-encoder-kb -I$PILK_SOFTWARE/view-manager" make client cpu_arch=generic SKIPREVENGTEST=1 SKIPWHEREAMISYSTEM=1 SKIPBT=1 SKIPQT=1 SKIPREADLINE=1 SKIPLINENOISE=1 SKIPLUASYSTEM=1 SKIPJANSSONSYSTEM=1 SKIPPYTHON=1 SKIPGD=1 -j