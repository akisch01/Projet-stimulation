package server;

import java.io.*;
import java.net.*;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.RemoteException;
import messages.MessageTemperature;
import java.util.ArrayList;
import java.util.List;

public class TemperatureManagerImpl extends UnicastRemoteObject implements TemperatureManager {
    private List<MessageTemperature> rooms;

    protected TemperatureManagerImpl() throws RemoteException {
        super();
        rooms = new ArrayList<>();
        startTCPServer();
    }

    @Override
    public MessageTemperature[] getAllRoomData() throws RemoteException {
        return rooms.toArray(new MessageTemperature[0]);
    }

    public void updateRoomData(String roomName, double temperature, int heatingLevel) {
        for (MessageTemperature room : rooms) {
            if (room.getRoomName().equals(roomName)) {
                rooms.remove(room);
                break;
            }
        }
        rooms.add(new MessageTemperature(roomName, temperature, heatingLevel));
        System.out.println("Données mises à jour pour la pièce : " + roomName + 
                           ", Température : " + temperature + 
                           ", Niveau de chauffage : " + heatingLevel);
    }

    private void startTCPServer() {
        new Thread(() -> {
            try (ServerSocket serverSocket = new ServerSocket(5001)) {
                System.out.println("Serveur TCP démarré sur le port 5001.");

                while (true) {
                    Socket clientSocket = serverSocket.accept();
                    new Thread(() -> handleClient(clientSocket)).start();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
    }

    private void handleClient(Socket clientSocket) {
        try (BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()))) {
            String data = in.readLine();
            System.out.println("Données reçues du système central : " + data); // Log supplémentaire

            String[] parts = data.split(";");
            if (parts.length == 3) {
                String roomName = parts[0];
                double temperature = Double.parseDouble(parts[1]);
                int heatingLevel = Integer.parseInt(parts[2]);

                updateRoomData(roomName, temperature, heatingLevel);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}