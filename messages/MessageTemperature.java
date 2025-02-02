package messages;

import java.io.Serializable;

public class MessageTemperature implements Serializable {
    private static final long serialVersionUID = 1L;

    private String roomName;
    private double temperature;
    private int heatingLevel;

    public MessageTemperature(String roomName, double temperature, int heatingLevel) {
        this.roomName = roomName;
        this.temperature = temperature;
        this.heatingLevel = heatingLevel;
    }

    public String getRoomName() {
        return roomName;
    }

    public double getTemperature() {
        return temperature;
    }

    public int getHeatingLevel() {
        return heatingLevel;
    }

    @Override
    public String toString() {
        return "Pièce : " + roomName +
               ", Température : " + temperature + "°C" +
               ", Niveau de chauffage : " + heatingLevel;
    }
}