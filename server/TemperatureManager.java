package server;

import java.rmi.Remote;
import java.rmi.RemoteException;
import messages.MessageTemperature;

public interface TemperatureManager extends Remote {
    MessageTemperature[] getAllRoomData() throws RemoteException;
}