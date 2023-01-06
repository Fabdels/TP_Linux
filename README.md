# TP_Linux


TP1 :

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

Certains fichiers permettent d'accéder à des périphériques de communication du microcontrôleur: dma, gpio, i2c-dev... et d'autres permettent d'accéder à la partie fpga du cyclone Soc.

1.4.3
Si on essaie d'exécuter l'exécutable généré, le shell revoie une erreur:
"cannot execute binary file: Exec format error" -> le format de cet executable ne convient pas à cet os





TP2 :


On peut utiliser mmap pour accéder directement à un registre et le modifier en bare-metal. Cependant cette méthode nécessite de connaître l'adresse mémoire hard. Cette adresse dépendant du device, cette méthode entraine des difficultés de portage.

2.2
sudo insmod hello.ko
sudo dmesg
  -> [2100.213443] Hello world!
 sudo lsmod | grep hello
  -> hello  16384 0 *rien*
sudo rmmod
  ->  [2199.213443] Bye Bye... (pas besoin de dmesg car le message est une KERN_ALERT)


- utilisation de paramètres au chargement du module

Pour définir la valeur d'un paramètre au chargement du module, il suffit d'écrire quelque chose de la forme :

insmod le_module param=2
  
  -> /proc/device -> major = 100 minor = 0 disponible!
sudo mknod /dev/le_driver_TP u 100 0






2.3
attention: l'export n'est valide que dans l'instance du shell ou il a été lancé et disparait au reboot
whereis arm-linux-gnueabihf-gcc
  -> arm-linux-gnueabihf-gcc /usr/bin/arm-linux-gnueabihf-gcc





TP3 :

On remplace 

ledr: gpio@0x100003000 {
compatible = "altr,pio-16.1", "altr,pio-1.0";
reg = <0x00000001 0x00003000 0x00000010>;
clocks = <&clk_50>;

par 

ledr: ensea {
compatible = "dev,ensea";
reg = <0x00000001 0x00003000 0x00000010>;
clocks = <&clk_50>;


Dans le nouveau device tree, dans bridge@0xc000000, on trouve le module ensea


3.1

Probe -> leds_probe
Appelée dès que le noyau trouve un nouveau device compatible avec notre driver.

Cette fonction :
Allume les LED (en accèdant au registre 0 dans le module ensea LEDs)
Initialise le device (crée un character file dans l'espace utilisateur)


Read -> ssize_t leds_read
Appelée dès qu'une opération de lecture a lieu sur un des character files.

Cette fonction :
Donne à l'utilisateur la valeur de la LED
(Si la copie de la valeur sur l'espace utilisateur échoue, affiche un message d'erreur)

Write -> ssize_t leds_write
Appelée dès qu'une opération d'écriture a lieu sur un des character files.

Cette fonction :
Récupère la nouvelle valeur de la LED (Les premiers octets des données fournies)
(Si la copie de la valeur depuis l'espace utilisateur échoue, affiche un message d'erreur)
Mets les LED 

Remove -> leds_remove
Appelée dès qu'un device utilisant ce driver est supprimé.
(Egalement appelée pour chaque device géré par notre driver lorsque celui-ci est supprimé
du système (avec la commande rmmod))

Cette fonction :
Eteint les LEDs
Supprime le character file de /dev





Compilation impossible dans le dossier partagé.
La compilation est à faire avec un makefile et les bons flags à ajouter.


3.2






