sudo pkill -f "console_commande"
sudo pkill -f "console_controle"
sudo pkill -f "ControlConsole"
sudo pkill -f "systeme_central"
sudo pkill -f "java.*ServerMain"
sudo pkill -f "java.*Air"
sudo pkill -f "chauffage"
sudo pkill -f "thermometre"

javac server/*.java messages/*.java console/ControlConsole.java air/*.java

java -cp .:server:messages server.ServerMain


tmux new-session -d -s projet 'java -cp .:air air.Air Salon 239.255.1.10 5001 15'
tmux split-window -h './chauffage/chauffage 127.0.0.1 5001 6001'
tmux split-window -v './thermometre/thermometre 239.255.1.10 5001 127.0.0.1 6001'
tmux select-layout tiled
tmux attach

sudo systemctl stop apache2
sudo /opt/lampp/lampp start 

java -cp .:server:messages server.ServerMain
java server.ServerMain

./systeme_central/systeme_central 239.255.255.250 5000 6000
export LD_LIBRARY_PATH=/usr/lib/jvm/java-17-openjdk-amd64/lib/server:$LD_LIBRARY_PATH
./systeme_central/systeme_central 239.255.255.250 5000 6000

./chauffage/chauffage 239.255.255.250 5000 7000

./thermometre/thermometre 239.255.255.250 5000 127.0.0.1 6000

java air.Air Salon 239.255.255.250 5000 20.0

./console/console_commande

./console/console_controle

java -cp .:messages:server console.ControlConsole
java console.ControlConsole


./systeme_central/systeme_central 239.255.255.250 5000 6000

./thermometre/thermometre 239.255.255.250 5000 127.0.0.1 6000

./chauffage/chauffage 239.255.255.250 5000 7000