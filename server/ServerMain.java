package server;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class ServerMain {
    public static void main(String[] args) {
        try {
            System.setProperty("java.rmi.server.hostname", "127.0.0.1");
            LocateRegistry.createRegistry(1100); // Port RMI
            TemperatureManagerImpl manager = new TemperatureManagerImpl();
            Registry registry = LocateRegistry.getRegistry(1100);
            registry.rebind("TemperatureManager", manager);
            System.out.println("Serveur RMI prêt sur 127.0.0.1:1100.");
        } catch (Exception e) {
            System.err.println("Erreur : " + e.getMessage());
        }
    }
}