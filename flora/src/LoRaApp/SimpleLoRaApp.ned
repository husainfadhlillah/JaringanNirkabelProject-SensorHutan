//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

package flora.LoRaApp;

import inet.applications.contract.IApp;

//
// TODO auto-generated module
//
simple SimpleLoRaApp like IApp
{
    parameters:
        @signal[LoRa_AppPacketSent](type=long); // optional
        @statistic[LoRa_AppPacketSent](source=LoRa_AppPacketSent; record=count);

        @signal[temperature](type=double);
        @statistic[temperatureValue](source=temperature; record=histogram,stats);
        @statistic[temperatureStats](source=temperature; record=min,max,mean);

        @signal[humidity](type=double);
        @statistic[humidityValue](source=humidity; record=histogram,stats);
        @statistic[humidityStats](source=humidity; record=min,max,mean);

        @signal[realTemperature](type=double);
        @signal[realHumidity](type=double);
        @signal[temperatureError](type=double);
        @signal[humidityError](type=double);

        @statistic[realTemp](source=realTemperature; record=histogram,stats);
        @statistic[realHum](source=realHumidity; record=histogram,stats);
        @statistic[tempError](source=temperatureError; record=histogram,stats);
        @statistic[humError](source=humidityError; record=histogram,stats);

        @signal[realFireDetected](type=double);
        @statistic[realFireDetection](source=realFireDetected; record=vector,count);

        int numberOfPacketsToSend = default(1);
        volatile double timeToFirstPacket @unit(s) = default(10s);
        volatile double timeToNextPacket @unit(s) = default(10s);
        double initialLoRaTP @unit(dBm) = default(14dBm);
        double initialLoRaCF @unit(Hz) = default(868MHz);
        int initialLoRaSF = default(12);
        double initialLoRaBW @unit(Hz) = default(125kHz);
        int initialLoRaCR = default(6);
        bool initialUseHeader = default(true);
        bool evaluateADRinNode = default(false);
        int dataSize @unit(B) = default(10B);

        @signal[fireDetected](type=double);
        @signal[tempNoise](type=double);
        @signal[humNoise](type=double);
        @signal[fireNoise](type=double);

        @statistic[fireDetection](source=fireDetected; record=vector,count);
        @statistic[tempNoiseStats](source=tempNoise; record=vector,histogram,stats);
        @statistic[humNoiseStats](source=humNoise; record=vector,histogram,stats);
        @statistic[fireNoiseStats](source=fireNoise; record=vector,histogram,stats);


    gates:
        input socketIn @labels(LoRaAppPacket/up);
        output socketOut @labels(LoRaAppPacket/down);
}
