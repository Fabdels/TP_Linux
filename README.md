# TP_Linux

df -h
/dev/root       3.0G  1.3G  1.5G  47% /
devtmpfs        375M     0  375M   0% /dev
tmpfs           376M     0  376M   0% /dev/shm
tmpfs           376M  9.8M  366M   3% /run
tmpfs           5.0M  4.0K  5.0M   1% /run/lock
tmpfs           376M     0  376M   0% /sys/fs/cgroup
tmpfs            76M     0   76M   0% /run/user/0

Seuls 3Go ont été alloués pour le système et la moitié est utilisée.

Après avoir étendu l'espace occupé par linux, la commande df -h donne:
Filesystem      Size  Used Avail Use% Mounted on
/dev/root        29G  1.3G   26G   5% /
devtmpfs        375M     0  375M   0% /dev
tmpfs           376M     0  376M   0% /dev/shm
tmpfs           376M  9.8M  366M   3% /run
tmpfs           5.0M  4.0K  5.0M   1% /run/lock
tmpfs           376M     0  376M   0% /sys/fs/cgroup
tmpfs            76M     0   76M   0% /run/user/0

Nous avons bien utilisé les 30 Go de la carte.

Dans /sys/class, les fichiers suivants sont disponibles:
bdi          f2h-dma-memory  graphics     input     mmc_host  regulator    sound       vc
block        fpga_bridge     hwmon        leds      mtd       rtc          spi_master  video4linux
bluetooth    fpga_manager    i2c-adapter  mdio_bus  net       scsi_device  spidev      vtconsole
devcoredump  fpga_region     i2c-dev      mem       pps       scsi_disk    tty
dma          gpio            ieee80211    misc      ptp       scsi_host    udc

Certains fichiers permettent d'accéder à des périphériques de communication du microcontrolleur: dma, gpio, i2c-dev... et d'autres permettent d'accéder à la partie fpga du cyclone Soc.

1.4.3
Si on essaie d'exécuter l'exécutable généré, le shell revoie une erreure:
"cannot execute binary file: Exec format error" -> le format de cet executable ne convient pas à cet os
