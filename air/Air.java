package air;

import java.net.*;
import java.io.*;

public class Air {
    private static final int BUFFER_SIZE = 1024;
    private String roomName;
    private InetAddress multicastAddress;
    private int multicastPort;
    private double externalTemperature;
    private double currentTemperature;

    public Air(String roomName, String multicastAddress, int multicastPort, double externalTemperature) throws IOException {
        this.roomName = roomName;
        this.multicastAddress = InetAddress.getByName(multicastAddress); // Utilisation du paramètre multicastAddress
        this.multicastPort = multicastPort; // Utilisation du paramètre multicastPort
        this.externalTemperature = externalTemperature;
        this.currentTemperature = externalTemperature;
    }

    public void simulate() {
        try (DatagramSocket socket = new DatagramSocket()) {
            System.out.println("Simulation de l'air pour la pièce : " + roomName + " démarrée.");
            byte[] buffer = new byte[BUFFER_SIZE];

            while (true) {
                String temperatureMessage = "TEMP " + currentTemperature;
                DatagramPacket packet = new DatagramPacket(
                    temperatureMessage.getBytes(), temperatureMessage.length(),
                    multicastAddress, multicastPort
                );
                socket.send(packet);
                System.out.println("Température envoyée : " + currentTemperature);

                Thread.sleep(2000);
            }
        } catch (IOException | InterruptedException e) {
            System.err.println("Erreur : " + e.getMessage());
        }
    }

    public static void main(String[] args) {
        if (args.length != 4) {
            System.out.println("Usage: java Air <roomName> <multicastAddress> <multicastPort> <externalTemperature>");
            return;
        }

        try {
            Air air = new Air(args[0], "239.255.255.250", 5000, Double.parseDouble(args[3]));
            air.simulate();
        } catch (Exception e) {
            System.err.println("Erreur lors de l'initialisation de l'air : " + e.getMessage());
        }
    }
}