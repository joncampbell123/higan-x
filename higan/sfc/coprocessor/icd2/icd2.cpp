#include <sfc/sfc.hpp>

namespace SuperFamicom {

ICD2 icd2;

#if defined(SFC_SUPERGAMEBOY)

#include "platform.cpp"
#include "interface.cpp"
#include "io.cpp"
#include "serialization.cpp"

auto ICD2::Enter() -> void {
  while(true) {
    if(scheduler.synchronizing()) GameBoy::system.runToSave();
    scheduler.synchronize();
    icd2.main();
  }
}

auto ICD2::main() -> void {
  if(r6003 & 0x80) {
    GameBoy::system.run();
    step(GameBoy::system._clocksExecuted);
    GameBoy::system._clocksExecuted = 0;
  } else {  //DMG halted
    stream->sample(0.0, 0.0);
    step(2);  //two clocks per audio sample
  }
  synchronize(cpu);
}

auto ICD2::init() -> void {
}

auto ICD2::load() -> bool {
  GameBoy::superGameBoy = this;
  GameBoy::system.load(&gameBoyInterface, GameBoy::System::Model::SuperGameBoy, cartridge.pathID());
  return cartridge.loadGameBoy();
}

auto ICD2::unload() -> void {
  GameBoy::system.save();
  GameBoy::system.unload();
}

auto ICD2::power() -> void {
  create(ICD2::Enter, system.colorburst() * 6.0 / 5.0);
  stream = Emulator::audio.createStream(2, frequency() / 2.0);
  stream->addLowPassFilter(20000.0, 3);
  stream->addHighPassFilter(20.0, 3);

  r6003 = 0x00;
  r6004 = 0xff;
  r6005 = 0xff;
  r6006 = 0xff;
  r6007 = 0xff;
  for(auto& r : r7000) r = 0x00;
  mltReq = 0;

  for(auto& n : output) n = 0xff;
  readBank = 0;
  readAddress = 0;
  writeBank = 0;
  writeAddress = 0;

  packetSize = 0;
  joypID = 3;
  joyp15Lock = 0;
  joyp14Lock = 0;
  pulseLock = true;

  GameBoy::system.init();
  GameBoy::system.power();
}

auto ICD2::reset() -> void {
  auto frequency = system.colorburst() * 6.0;
  create(ICD2::Enter, frequency / 5);

  r6003 = 0x00;
  r6004 = 0xff;
  r6005 = 0xff;
  r6006 = 0xff;
  r6007 = 0xff;
  for(auto& r : r7000) r = 0x00;
  mltReq = 0;

  for(auto& n : output) n = 0xff;
  readBank = 0;
  readAddress = 0;
  writeBank = 0;
  writeAddress = 0;

  packetSize = 0;
  joypID = 3;
  joyp15Lock = 0;
  joyp14Lock = 0;
  pulseLock = true;

  GameBoy::system.init();
  GameBoy::system.power();
}

#endif

}
