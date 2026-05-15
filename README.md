# GD4_CA3_Dominik_Domalip

#### Comparison CA2 vs CA3
For both projects I worked with tank style game, in the CA2 I was experimenting little more because I had more time for it. In CA3 it took a long time to get the architecture correctly and actually to run the game. 

###### CA2 (TCP Focus)
- Extension of an existing SFML single player codebase
- Integrated multiplayer through game states - `MultiplayerGameState`
- Dedicated 'GameServer' running in its own thread
- Using SFML's high-level `sf::TcpListener` and `sf::TcpSocket`

###### CA3 (UDP Focus)
- Clean architecture with separate dedicated executables `SERVER` and `CLIENT`
- Introduces  `NetworkManager`, `ReplicationManager`, `ClientProxy`, and network ID system
- Pure UDP implementation usign custom `OutputMemoryBitStream` / `InputMemoryBitStream` instead of the SFML networking

| Aspect                    | CA2 (TCP)                                      | CA3 (UDP)                                              |
|---------------------------|------------------------------------------------|--------------------------------------------------------|
| **Transport**             | TCP (reliable, ordered, connection-oriented)  | UDP (unreliable, connectionless)                       |
| **Reliability**           | Fully handled by TCP                           | Manually implemented (sequencing, acks, DeliveryNotificationManager) |
| **Serialization**         | SFML `sf::Packet`                              | Custom bitstreams for maximum efficiency               |
| **Replication**           | Full world state sent regularly                | Dirty-bit replication (only changed data)              |
| **Latency**               | Higher (TCP overhead + head-of-line blocking)  | Lower – better suited for real-time games              |
| **Connection**            | Persistent sockets                             | Address-based `ClientProxy` system                     |



# Resources
- itch.io. (2026). Crosshair Pack. [online] Available at: https://kenney-assets.itch.io/crosshair-pack?download [Accessed 15 May 2026].
- Freesound. (2025). Session Beat by Lit1onion. [online] Available at: https://freesound.org/people/Lit1onion/sounds/784081/ [Accessed 17 Feb. 2026].
- Freesound. (2018). Mushroom Background Music by Sunsai. [online] Available at: https://freesound.org/people/Sunsai/sounds/415804/ [Accessed 17 Feb. 2026]
- freesound.org. (n.d.). Freesound - Tank Firing by qubodup. [online] Available at: https://freesound.org/people/qubodup/sounds/168707/ [Accessed 17 Feb. 2026]
- Freesound. (2016). Sizzling boom by AceOfSpadesProduc100. [online] Available at: https://freesound.org/people/AceOfSpadesProduc100/sounds/340960/ [Accessed 17 Feb. 2026]
- Freesound. (2020). button-selected.wav by StavSounds. [online] Available at: https://freesound.org/people/StavSounds/sounds/546079/ [Accessed 17 Feb. 2026]
- Freesound. (2020). Select, Granted 04.wav by LilMati. [online] Available at: https://freesound.org/people/LilMati/sounds/515823/ [Accessed 17 Feb. 2026]
- Freesound. (2026). Bullet_Impact_2 by toxicwafflezz. [online] Available at: https://freesound.org/people/toxicwafflezz/sounds/150838/ [Accessed 17 Feb. 2026] 
- Freesound. (2023). Car Impact Hit, Shatter, No Glass by PNMCarrieRailfan. [online] Available at: https://freesound.org/people/PNMCarrieRailfan/sounds/681527/ [Accessed 17 Feb. 2026]
- Freesound. (2020). Powerup 04.wav by LilMati. [online] Available at: https://freesound.org/people/LilMati/sounds/503520/ [Accessed 17 Feb. 2026]
- Freesound. (2025). Smashed/demolished brick wall crumbling/caving in by Squirrel_404. [online] Available at: https://freesound.org/people/Squirrel_404/sounds/829103/ [Accessed 17 Feb. 2026]
- The Spriters Resource. (2026). Box Backgrounds - Pokémon Black / White - DS / DSi. [online] Available at: https://www.spriters-resource.com/ds_dsi/pokemonblackwhite/asset/34025/ [Accessed 17 Feb. 2026]
- itch.io. (n.d.). Stones & Brick Textures by Pucci Games. [online] Available at: https://pucci-games.itch.io/stones-brick-textures [Accessed 17 Feb. 2026]
- Pngegg.com. (2026). Free download | Computer hardware, Gun Turret, hardware, machine png | PNGEgg. [online] Available at: https://www.pngegg.com/en/png-tovsr/download [Accessed 17 Feb. 2026]#
- Moreira, Artur, Jan Haller, and Henrik Vogelius Hansson. SFML Game Development. Packt Publishing, 2013
- Gemini. (n.d.). Google Gemini picture generation. [online] Available at: https://gemini.google.com/app?hl=en_GB.
- Clipart Library (2026). Illustration. [online] Clipart-library.com. Available at: https://clipart-library.com/clip-art/49-495275_bullets-clipart-sprite-bullets-sprite.htm [Accessed 20 Feb. 2026].
- Freesound. (2022). TF_movie_Optimus_inspired_laser_sound_effect_01_2022 by Artninja. [online] Available at: https://freesound.org/people/Artninja/sounds/784935/ [Accessed 20 Feb. 2026].
- Freesound. (2016). Launching 1 by AceOfSpadesProduc100. [online] Available at: https://freesound.org/people/AceOfSpadesProduc100/sounds/334268/ [Accessed 20 Feb. 2026].

