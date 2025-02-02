package console;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Arrays;

import messages.MessageTemperature;
import server.TemperatureManager;

public class ControlConsole {
    public static void main(String[] args) {
        try {
            Registry registry = LocateRegistry.getRegistry("127.0.0.1", 1100); // Connexion RMI
            System.out.println("Connecté au serveur RMI.");
            TemperatureManager manager = (TemperatureManager) registry.lookup("TemperatureManager");
            
            while (true) {
                MessageTemperature[] roomData = manager.getAllRoomData();
                System.out.println("Données reçues du serveur RMI : " + Arrays.toString(roomData));
                System.out.println("\n=== État des pièces ===");
                for (MessageTemperature data : roomData) {
                    System.out.println(data);
                }
                System.out.println("=======================");
                Thread.sleep(5000);
            }
        } catch (Exception e) {
            System.err.println("Erreur : " + e.getMessage());
        }
    }
}