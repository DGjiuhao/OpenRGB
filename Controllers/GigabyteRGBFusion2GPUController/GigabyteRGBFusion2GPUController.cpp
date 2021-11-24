/*-----------------------------------------*\
|  GigabyteRGBFusion2GPUController.cpp      |
|                                           |
|  Driver for Gigabyte Aorus RGB Fusion2 GPU|
|  lighting controller                      |
|                                           |
|  based on original by                     |
|  Adam Honse (CalcProgrammer1)             |
\*-----------------------------------------*/

#include <chrono>
#include <thread>
#include "GigabyteRGBFusion2GPUController.h"
#include "LogManager.h"

using namespace std::chrono_literals;

RGBFusion2GPUController::RGBFusion2GPUController(i2c_smbus_interface* bus, rgb_fusion_dev_id dev)
{
    this->bus = bus;
    this->dev = dev;
}

RGBFusion2GPUController::~RGBFusion2GPUController()
{

}

std::string RGBFusion2GPUController::GetDeviceLocation()
{
    std::string return_string(bus->device_name);
    char addr[5];
    snprintf(addr, 5, "0x%02X", dev);
    return_string.append(", address ");
    return_string.append(addr);
    return("I2C: " + return_string);
}

void RGBFusion2GPUController::SaveConfig()
{
    uint8_t data_pkt[8] = { 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    bus->i2c_write_block(dev, sizeof(data_pkt), data_pkt);
}

void RGBFusion2GPUController::SetMode(uint8_t zone, uint8_t mode, fusion2_config zone_config, uint8_t mystery_flag)
{
    uint8_t zone_pkt[8] = { RGB_FUSION2_GPU_REG_MODE, mode, zone_config.speed, zone_config.brightness, mystery_flag, (zone + 1), 0x00, 0x00 };
    bus->i2c_write_block(dev, sizeof(zone_pkt), zone_pkt);

    uint8_t zone_pkt2[8] = { RGB_FUSION2_GPU_REG_COLOR, RGBGetRValue(zone_config.colors[0]), RGBGetGValue(zone_config.colors[0]), RGBGetBValue(zone_config.colors[0]), (zone + 1), 0x00, 0x00, 0x00 };
    bus->i2c_write_block(dev, sizeof(zone_pkt2), zone_pkt2);
}

void RGBFusion2GPUController::SetZone(uint8_t zone, uint8_t mode, fusion2_config zone_config)
{
    std::string mode_name;
    uint8_t mystery_flag = 0x00;

    switch(mode)
    {
        case RGB_FUSION2_GPU_MODE_STATIC:
            {
                SetMode(zone, mode, zone_config, mystery_flag);
            }
            break;

        case RGB_FUSION2_GPU_MODE_BREATHING:
            {
                SetMode(zone, mode, zone_config, mystery_flag);
            }
            break;

        case RGB_FUSION2_GPU_MODE_COLOR_CYCLE:
            {
                uint8_t zone_pkt[8] = { RGB_FUSION2_GPU_REG_MODE, mode, zone_config.speed, zone_config.brightness, mystery_flag, (zone + 1), 0x00, 0x00 };
                bus->i2c_write_block(dev, sizeof(zone_pkt), zone_pkt);
            } 
            break;

        case RGB_FUSION2_GPU_MODE_GRADIENT:
            {
                mystery_flag = 0x08;
                SetMode(zone, mode, zone_config, mystery_flag);
            }
            break;

        case RGB_FUSION2_GPU_MODE_FLASHING:
            {
                SetMode(zone, mode, zone_config, mystery_flag);
            }
            break;

        case RGB_FUSION2_GPU_MODE_DUAL_FLASHING:
            {
                SetMode(zone, mode, zone_config, mystery_flag);
            }
            break;

        case RGB_FUSION2_GPU_MODE_WAVE:
            {
                SetMode(zone, mode, zone_config, mystery_flag);
            }
            break;

        case RGB_FUSION2_GPU_MODE_COLOR_SHIFT: 
            {
                mystery_flag = zone_config.numberOfColors;
                uint8_t zone_pkt[8] = { RGB_FUSION2_GPU_REG_MODE, mode, zone_config.speed, zone_config.brightness, mystery_flag, (zone + 1), 0x00, 0x00 };
                bus->i2c_write_block(dev, sizeof(zone_pkt), zone_pkt);

                uint8_t bank = (0xB0 + (zone * 4));
                uint8_t zone_pktA[8] = { bank, mode, RGBGetRValue(zone_config.colors[0]), RGBGetGValue(zone_config.colors[0]), RGBGetBValue(zone_config.colors[0]), RGBGetRValue(zone_config.colors[1]), RGBGetGValue(zone_config.colors[1]), RGBGetBValue(zone_config.colors[1]) };
                bus->i2c_write_block(dev, sizeof(zone_pktA), zone_pktA);
                uint8_t zone_pktB[8] = { (bank + 1), mode, RGBGetRValue(zone_config.colors[2]), RGBGetGValue(zone_config.colors[2]), RGBGetBValue(zone_config.colors[2]), RGBGetRValue(zone_config.colors[3]), RGBGetGValue(zone_config.colors[3]), RGBGetBValue(zone_config.colors[3]) };
                bus->i2c_write_block(dev, sizeof(zone_pktB), zone_pktB);
                uint8_t zone_pktC[8] = { (bank + 2), mode, RGBGetRValue(zone_config.colors[4]), RGBGetGValue(zone_config.colors[4]), RGBGetBValue(zone_config.colors[4]), RGBGetRValue(zone_config.colors[5]), RGBGetGValue(zone_config.colors[5]), RGBGetBValue(zone_config.colors[5]) };
                bus->i2c_write_block(dev, sizeof(zone_pktC), zone_pktC);
                uint8_t zone_pktD[8] = { (bank + 3), mode, RGBGetRValue(zone_config.colors[6]), RGBGetGValue(zone_config.colors[6]), RGBGetBValue(zone_config.colors[6]), RGBGetRValue(zone_config.colors[7]), RGBGetGValue(zone_config.colors[7]), RGBGetBValue(zone_config.colors[7]) };
                bus->i2c_write_block(dev, sizeof(zone_pktD), zone_pktD);
            }
            break;

        case RGB_FUSION2_GPU_MODE_TRICOLOR:
            {
                mystery_flag = 0x08;
                uint8_t zone_pkt[8] = { RGB_FUSION2_GPU_REG_MODE, mode, zone_config.speed, zone_config.brightness, mystery_flag, (zone + 1), 0x00, 0x00 };
                bus->i2c_write_block(dev, sizeof(zone_pkt), zone_pkt);

                uint8_t bank = (0xB0 + (zone * 4));
                uint8_t zone_pktA[8] = { bank, mode, RGBGetRValue(zone_config.colors[0]), RGBGetGValue(zone_config.colors[0]), RGBGetBValue(zone_config.colors[0]), RGBGetRValue(zone_config.colors[1]), RGBGetGValue(zone_config.colors[1]), RGBGetBValue(zone_config.colors[1]) };
                bus->i2c_write_block(dev, sizeof(zone_pktA), zone_pktA);
                uint8_t zone_pktB[8] = { (bank + 1), mode, RGBGetRValue(zone_config.colors[2]), RGBGetGValue(zone_config.colors[2]), RGBGetBValue(zone_config.colors[2]), RGBGetRValue(zone_config.colors[3]), RGBGetGValue(zone_config.colors[3]), RGBGetBValue(zone_config.colors[3]) };
                bus->i2c_write_block(dev, sizeof(zone_pktB), zone_pktB);
                uint8_t zone_pktC[8] = { (bank + 2), mode, RGBGetRValue(zone_config.colors[4]), RGBGetGValue(zone_config.colors[4]), RGBGetBValue(zone_config.colors[4]), RGBGetRValue(zone_config.colors[5]), RGBGetGValue(zone_config.colors[5]), RGBGetBValue(zone_config.colors[5]) };
                bus->i2c_write_block(dev, sizeof(zone_pktC), zone_pktC);
                uint8_t zone_pktD[8] = { (bank + 3), mode, RGBGetRValue(zone_config.colors[6]), RGBGetGValue(zone_config.colors[6]), RGBGetBValue(zone_config.colors[6]), RGBGetRValue(zone_config.colors[7]), RGBGetGValue(zone_config.colors[7]), RGBGetBValue(zone_config.colors[7]) };
                bus->i2c_write_block(dev, sizeof(zone_pktD), zone_pktD);
            }
            break;
        
        default:
            {
                LOG_TRACE("[%s] Mode %02d not found", "fusion2 gpu", mode);
            }
            break;
    }
}
