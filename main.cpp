#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <algorithm> // Для std::transform, std::reverse
#include <cmath>     // Для std::sin

// Зчитування RAW файлу у вектор
std::vector<int16_t> readAudio(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
        if (!file) {
                std::cerr << "Помилка відкриття файлу: " << filename << std::endl;
                        return {};
                            }
                                // Переходимо в кінець файлу, щоб дізнатися його розмір
                                    file.seekg(0, std::ios::end);
                                        std::streamsize size = file.tellg();
                                            file.seekg(0, std::ios::beg);

                                                // Створюємо вектор відповідного розміру
                                                    std::vector<int16_t> buffer(size / sizeof(int16_t));

                                                        // Зчитуємо дані
                                                            file.read(reinterpret_cast<char*>(buffer.data()), size);
                                                                return buffer;
                                                                }

                                                                // Запис змінених даних назад у файл
                                                                void writeAudio(const std::string& filename, const std::vector<int16_t>& buffer) {
                                                                    std::ofstream file(filename, std::ios::binary);
                                                                        file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(int16_t));
                                                                        }

                                                                        int main() {
                                                                            // 1. Вичитуємо оригінальний файл у вектор
                                                                                std::string inputFile = "audio_raw.raw";
                                                                                    std::vector<int16_t> originalData = readAudio(inputFile);
                                                                                        
                                                                                            if (originalData.empty()) {
                                                                                                    return 1; // Завершуємо програму, якщо файл не знайдено
                                                                                                        }

                                                                                                            std::cout << "Файл успішно зчитано. Обробка даних..." << std::endl;

                                                                                                                // --- ЕФЕКТ 1: Збільшення гучності (Множення на 2) ---
                                                                                                                    std::vector<int16_t> volData = originalData;
                                                                                                                        std::transform(volData.begin(), volData.end(), volData.begin(), [](int16_t sample) {
                                                                                                                                // Використовуємо int32_t для перевірки переповнення, щоб уникнути тріску
                                                                                                                                        int32_t increased = sample * 2; 
                                                                                                                                                if (increased > 32767) return (int16_t)32767;
                                                                                                                                                        if (increased < -32768) return (int16_t)-32768;
                                                                                                                                                                return (int16_t)increased;
                                                                                                                                                                    });
                                                                                                                                                                        writeAudio("audio_volume.raw", volData);

                                                                                                                                                                            // --- ЕФЕКТ 2: Реверс запису ---
                                                                                                                                                                                std::vector<int16_t> revData = originalData;
                                                                                                                                                                                    std::reverse(revData.begin(), revData.end());
                                                                                                                                                                                        writeAudio("audio_reverse.raw", revData);

                                                                                                                                                                                            // --- ЕФЕКТ 3: Bit-crushing ---
                                                                                                                                                                                                std::vector<int16_t> crushData = originalData;
                                                                                                                                                                                                    std::transform(crushData.begin(), crushData.end(), crushData.begin(), [](int16_t sample) {
                                                                                                                                                                                                            return (sample >> 12) << 12;
                                                                                                                                                                                                                });
                                                                                                                                                                                                                    writeAudio("audio_bitcrush.raw", crushData);

                                                                                                                                                                                                                        // --- ЕФЕКТ 4: Distortion (Кліппінг) ---
                                                                                                                                                                                                                            std::vector<int16_t> distData = originalData;
                                                                                                                                                                                                                                int16_t threshold = 10000; // Можете змінити поріг
                                                                                                                                                                                                                                    std::transform(distData.begin(), distData.end(), distData.begin(), [threshold](int16_t sample) {
                                                                                                                                                                                                                                            if (sample > threshold) return threshold;
                                                                                                                                                                                                                                                    if (sample < -threshold) return (int16_t)-threshold;
                                                                                                                                                                                                                                                            return sample;
                                                                                                                                                                                                                                                                });
                                                                                                                                                                                                                                                                    writeAudio("audio_distortion.raw", distData);

                                                                                                                                                                                                                                                                        // --- ЕФЕКТ 5: Tremolo ---
                                                                                                                                                                                                                                                                            std::vector<int16_t> tremoloData = originalData;
                                                                                                                                                                                                                                                                                double freq = 0.0002; // Частота модуляції (можна підібрати на слух)
                                                                                                                                                                                                                                                                                    int i = 0; // Індекс для формули
                                                                                                                                                                                                                                                                                        std::transform(tremoloData.begin(), tremoloData.end(), tremoloData.begin(), [&i, freq](int16_t sample) {
                                                                                                                                                                                                                                                                                                double mod = 0.75 + 0.25 * std::sin(i * freq);
                                                                                                                                                                                                                                                                                                        i++;
                                                                                                                                                                                                                                                                                                                return static_cast<int16_t>(sample * mod);
                                                                                                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                                                                                                        writeAudio("audio_tremolo.raw", tremoloData);

                                                                                                                                                                                                                                                                                                                            // --- ЕФЕКТ 6: Ring Modulator ---
                                                                                                                                                                                                                                                                                                                                std::vector<int16_t> ringData = originalData;
                                                                                                                                                                                                                                                                                                                                    int j = 0;
                                                                                                                                                                                                                                                                                                                                        std::transform(ringData.begin(), ringData.end(), ringData.begin(), [&j](int16_t sample) {
                                                                                                                                                                                                                                                                                                                                                if (j % 2 == 0) sample = -sample; // Інвертуємо кожен другий семпл
                                                                                                                                                                                                                                                                                                                                                        j++;
                                                                                                                                                                                                                                                                                                                                                                return sample;
                                                                                                                                                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                                                                                                                                                        writeAudio("audio_ring_modulator.raw", ringData);

                                                                                                                                                                                                                                                                                                                                                                            std::cout << "Всі файли успішно згенеровано!" << std::endl;

                                                                                                                                                                                                                                                                                                                                                                                return 0;
                                                                                                                                                                                                                                                                                                                                                                                }