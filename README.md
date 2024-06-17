Bu uygulama, Qt framework ve OpenCV kütüphanesi kullanarak gerçek zamanlı nesne tespiti ve sınıflandırması gerçekleştiren bir masaüstü uygulamasıdır. Uygulama, YOLO (You Only Look Once) modelini kullanarak video akışındaki nesneleri tespit eder ve sınıflandırır.

### Temel Bileşenler ve İşleyiş:

1. **Kullanıcı Arayüzü:**
   - Kullanıcı arayüzü (UI) Qt Designer ile oluşturulmuştur.
   - Arayüzde bir video gösterim alanı (QLabel) ve iki düğme (startButton ve stopButton) bulunmaktadır. 
   - Kullanıcı, "Başlat" düğmesine tıkladığında kamera akışı başlar ve "Durdur" düğmesine tıkladığında ise durur.

2. **OpenCV ve YOLO Entegrasyonu:**
   - OpenCV'nin dnn (Deep Neural Network) modülü kullanılarak YOLO modeli yüklenir.
   - YOLO modelinin yapılandırma (cfg) ve ağırlık (weights) dosyaları ile sınıf isimlerini içeren dosya (coco.names) uygulamaya dahil edilmiştir.

3. **Video İşleme:**
   - Video akışı OpenCV VideoCapture nesnesi ile alınır.
   - Timer kullanılarak belirli aralıklarla (30 ms) video akışından kareler alınır ve işlenir.
   - Her kare üzerinde YOLO modeli ile nesne tespiti yapılır.

4. **Nesne Tespiti ve Görselleştirme:**
   - Her kare üzerinde YOLO modeli çalıştırılarak tespit edilen nesneler sınıflandırılır ve çerçevelenir.
   - Tespit edilen nesneler kare üzerinde dikdörtgenler ve sınıf isimleri ile görselleştirilir.

5. **Nesne Bilgisi Görüntüleme:**
   - Tespit edilen nesneler ve bu nesnelerin sayıları QLabel üzerinde metin olarak gösterilir.
   - Uygulama, her yeni kare işleminde tespit edilen nesneleri güncelleyerek kullanıcıya anlık bilgi sağlar.

6. **Kod Yapısı:**
   - `MainWindow` sınıfı, uygulamanın ana işlevselliğini ve arayüz etkileşimlerini yönetir.
   - `startVideo` ve `stopVideo` fonksiyonları video akışını başlatır ve durdurur.
   - `processFrameAndUpdateGUI` fonksiyonu, her timer tetiklemesinde yeni bir kareyi işler, nesne tespiti yapar ve sonuçları günceller.
   - `detectObjects` fonksiyonu, YOLO modelini kullanarak nesne tespiti gerçekleştirir.
   - `drawPred` fonksiyonu, tespit edilen nesneleri kare üzerinde çizer.

Bu uygulama, bilgisayar görüsü ve nesne tespiti konularında pratik bir örnek teşkil eder. Qt ve OpenCV entegrasyonu ile masaüstü uygulamalarında gerçek zamanlı görüntü işleme ve nesne tespiti gibi gelişmiş işlevler gerçekleştirilebilir.
