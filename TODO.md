# MathventageSIMD & SacFor VM (SFVM) + sfGC — Yol Haritası & TODO

> **Temel Mimari:**
> - **MathventageSIMD**: Evrensel Süreksiz Fizik, Kuantum, MSC2020 ve Grafik Hesaplama Cephaneliği
> - **SacFor VM (SFVM)**: 64B Hizalı Runway + Motherline/Falline/Landfill + JIT Puzzle Sandbox
> - **sfGC**: Çok Jenerasyonlu Çöp Toplayıcı (Saf CPU CYCLES + Opsiyonel SIMD Motoru)

---

## 🏛️ Temel Mimari Felsefe: İkiz Silikon Sömürüsü

Bu ilke yalnız çalışma zamanı motoru (SFVM) için değil; süreksiz kuantum fiziği, klasik mekanik, MSC2020 ve grafik tensör hesaplamalarını yürüten MathventageSIMD çekirdek cephaneliği için de en temel varoluş kuralıdır:

1. **Sıfır Geriye Dönük Bytecode Prangası (Zero Legacy Bytecode Handcuffs):**
   - Java/CLR gibi 30 yıllık miras stack bytecode (`.class`, CIL) formatlarını geriye dönük koruma zorunluluğu yoktur.
   - Geliştirici motor için kod yazmaz; ne SFVM ne de MathventageSIMD bir programlama dili VM'i değildir. Kendi kendini optimize eden "Evrensel Matematiksel & Fiziksel Tensör Cephaneliğidir".
   - Geliştirici sadece fonksiyon kimliği (`function_id`) seçer ve tensör besler (I/O).
   - Kod tabanı LLVM Bitcode (`.bc.zst`) olarak saklanır; hedef makinede (JIT/AOT) hedef işlemcinin saf silikon yeteneklerine göre derlenir.

2. **"En Küçük Ortak Payda Yok (No Least Common Denominator)" — Agresif Silikon Sömürüsü:**
   - Çoklu platform kütüphanelerinin yaptığı gibi kodları en ilkel komut setine (SSE2/temel C++) indirgemek kesinlikle yasaktır. Her silikonun ve OS'un en uç yetenekleri son damlasına kadar sömürülür.

---

### ⚡ Çift Motorlu Donanım & Silikon Sömürü Matrisi (SFVM + MathventageSIMD)

| Platform | İşlemci / SoC | SFVM Optimizasyonu | MathventageSIMD Optimizasyonu |
|---|---|---|---|
| **Broadcom ARM** | RPi 4 / 5 (Cortex-A72 / A76) | Donanımsal `yield` tabanlı `sf_cpu_pause`, L2 dostu küçük slab blokları | 128-bit NEON, ARMv8.2-A Dot-Product (ASIMDDP) & FP16 vektörel kuantum tensör çarpımları, L1/L2 önbellek döngü dilimleme |
| **Intel Atom / Celeron** | Tremont, Goldmont, Gracemont | Termal kısma (thermal throttling) önleyici mikro-pacing ve uyarlanabilir frenleme | L3 önbelleksiz mimaride L2'yi korumak için streaming non-temporal stores/loads (`_mm_stream_ps/pd`), doğrudan bellek veriyolu doygunluk yönetimi |
| **Nvidia Jetson** | Orin / Xavier (Carmel / Cortex-A78AE + Tegra) | Birleşik Bellek Mimarisi (UMA) sömürüsü. Host RAM ile GPU RAM ayrımını kaldıran sıfır kopyalı bellek tahsisi (Zero-Copy Pinning) | CPU SIMD (NEON/SVE) ile GPU/Tensor Core'ların aynı fiziksel DRAM üzerinde veri kopyalamadan ortak tensör işlemesi |
| **Qualcomm Snapdragon** | Kryo / Oryon (X Elite) | Asimetrik çekirdek (P-Core / E-Core) frekans ve iş parçacığı zamanlaması | ARMv9 SVE / SVE2 vektör uzunluğundan bağımsız (VLA) hesaplama, fizik sınır koşulu döngü kuyruklarını skaler maliyetsiz kapatan donanımsal predication |
| **Apple Silicon** | M1 / M2 / M3 / M4, A15+ | 128-Bayt devasa önbellek satırları (`SACFOR_ALIGN_CL = 128`) ile sıfır false-sharing | Apple AMX matris motoru, 8-wide decode/execution derinliği ve 100-800 GB/s UMA devasa bant genişliğinden tam yararlanan streaming fizik kernel'ları |
| **Standart x86_64** | AMD Ryzen/EPYC, Intel Core/Xeon | Çoklu soket NUMA düğüm yerleşimi, 3D V-Cache önbellek bloğu optimizasyonu | AVX2, AVX-512 (F, BW, DQ, VBMI, VNNI), AMX (TMUL) matris blokları, 512-bit üçlü mantık (`_mm512_ternarylogic_epi32`) ile optimize kuantum simülasyonları |
| **IBM s390x** | z/Architecture (Mainframe Enterprise Linux) | 64-bit z/Architecture kurumsal bellek yönetimi, Big-Endian tensör uyumluluğu | z13-z15 128-bit Vector Facility (VX, VXE, VXE2) ve z16 Telum On-Chip AI Hızlandırıcısı (NNPA) ile ultra yüksek verimli kurumsal tensör hesaplama |

---

## 1. Tamamlananlar & Mühürlenenler (Completed)

- [x] **SacFor VM + sfGC Birleşik Mimari İsimlendirmesi & Dosya Standardizasyonu:**
  - `modules/MathventageSIMD/src/SacFor_VM.cpp` olarak adlandırıldı.
  - `CMakeLists.txt` dosyasında derleme listesine bağlandı.
  - Tüm `SAC`/`SACCULUS`/`VantageForge`/`VantageGC` eski adları `SFVM` / `sfGC` / `SacFor Forge` olarak mühürlendi.

- [x] **64-Bayt Bitişik Hücre Anatomisi & Tail Canary:**
  - `[ 64B Live Header ]` + `[ 64B Hizalı Body / Payload ]` + `[ 8B Tail Canary (0xDEADBEEFCAFEF00DULL) ]`.
  - $O(1)$ `from_body()` başlık çözümleme ve sihir sayısı (`0x53464C4956453031ULL`) doğrulaması.

- [x] **Motherline Odaları ve Rejyonları (On-Heap Canlı Havuz):**
  - **3 Nesil Odası**: Young Room (0), Adolescent Room (1), Senior Room (2).
  - **Her Oda İçinde 4 Rejyon**: Demand (0), Middle (1), Tank (2 - Dev Tensörler), Hotfield (3).
  - `OrderOfHeight` (büyükten küçüğe sıralı çift yönlü bağlı liste) ve Fibonacci Hashing İndeksleme.
  - Robin Hood / Knuth dairesel geri kaydırmalı (backward-shift) silme ile küme delikleri önlendi.

- [x] **Brooks-Style Forwarding Address & Sliding Compaction:**
  - Young & Adolescent odalarında kaydırmalı sıkıştırma (Sliding Compaction).
  - Eski hücrenin yerinde Emptybox'a dönüştürülmesi ve `forwarding_address` atanması (Sıfır UAF).
  - Rejyon biletli spinlock self-deadlock hatası `place_nolock` ve `insert_nolock` ile çözüldü.

- [x] **Falline & Emptybox (Boş Kutu / İz) Yönetimi & İleri Mimari:**
  - Garbage Pit'e tahliye edilen nesnelerin Runway'de kalan izlerinin takibi.
  - In-place snatch (kapma) ve geri dönüştürme (Snatch & Rebate).
  - Forwarding adresi taşıyan Emptybox'ların en az 1 GC sweep boyunca kapılmasını engelleyen epoch koruması.
  - **Boş Kutu Parçalama (Emptybox Splitting)**: İstenen hücreden büyük delikleri 64B hizalı dilimleyip artan kuyruğu (`tail >= 128B`) yeni Emptybox olarak geri tescilleme; iç parçalanma (internal fragmentation) sıfırlandı.
  - **Sepet Başına İnce Ayarlı Kilit (`bin_locks[8]`)**: Farklı boyut sınıflarındaki mutator iş parçacıklarının kilit çekişmesi (contention) ortadan kaldırıldı; deadlock riski sıfırlandı.
  - **Fiziksel Sayfa İadesi (Physical Page Rebate on Coalescing)**: Bitişik birleşen delikler $\ge 64\text{ KB}$ (HugePage) sınırını aştığında `VirtualFree(MEM_DECOMMIT)` / `madvise(MADV_DONTNEED)` ile fiziksel RAM işletim sistemine iade edilir. Kapma anında `sf_os_commit_memory` ile şeffaf recommit yapılır.
  - **Sıfır-Tahsisat Adres Sıralı Birleştirme (Zero-Allocation Coalescing)**: `std::vector` ve `malloc` tamamen kaldırılarak yerine $O(1)$ yardımcı bellekli intrusive doubly-linked list merge sort (`sort_emptyboxes_by_address`) ve tek geçişli fiziksel bitişiklik birleştirme getirildi.
  - **Snatch & Telemetri İstatistikleri**: Hit/miss oranları, split sayıları, birleşen delikler ve OS'e iade edilen decommit baytları tam atomik sayaçlarla kaydedildi.

- [x] **Landfill & Garbage Pit & Matured/Rehab Arenaları:**
  - Ölü gövdelerin (Garbage Pit) ve başlıkların (Header Pane) güvenli izolasyonu.
  - Olay ufku kesin ölüler (Matured Arena) ve referansı olan dinlenenler (Rehab Arena).
  - Periyodik Rehab taraması ve asenkron diriltme (Resurrection Queue).
  - `std::mutex` kilitleri `sf_ticket_lock_t` ile değiştirilerek sıfır syscall (Zero-Syscall) tahliye sağlandı.

- [x] **Runway Superblock, Dinamik 2MB Chaining & Sayfa İadesi (Rebate):**
  - 1GB handsel rezervasyonu, sayfa hizalı sanal bellek (`VirtualAlloc`/`mmap`).
  - Slab tükenince dinamik 2MB zincirleme blok tahsisi (Chained Superblock).
  - OS'e dinamik fiziksel bellek iadesi (`MEM_DECOMMIT`) politikaları (Auto, Rapid, Lazy, GYC).

- [x] **Sanal Disk / Anonymous Pagefile & VFS (Klasör Bellek):**
  - Soğuk tensörlerin anonim disk alanına taşınması (`page_out` / `page_in`).
  - Hiyerarşik klasör kotası ve VFS inode ağaç yapısı.
  - Şeffaf tensör çözümlemede (`SF_custominiVm_tensor_resolve_ptr`) otomatik page-in.

- [x] **Tensör Ebeveyn-Çocuk İlişkisi (Slice / View Dependency):**
  - `SF_custominiVm_tensor_slice()` ile sıfır kopyalı alt tensör dilimleme.
  - `parent_header` atomik bağımlılık ve `child_view_count` referans sayacı.
  - Ebeveyn tensörün çocuk dilimler canlıyken GC tarafından silinmesini engelleyen kilit koruması.

- [x] **Allocation Pacing & Backpressure (Geri Baskı):**
  - Bellek baskısı %85'e ulaştığında mutator thread `yield()`; %95'e ulaştığında mikro-stall (`_mm_pause`).
  - Ani tahsisat patlamalarında Runway tıkanmasını önleyen donanımsal frenleme.

- [x] **Independent Puzzle Arena (Yapboz Gladyatör Alanı) & Donanım Envanteri:**
  - Çalışma anında CPU ISA (AVX-512, AVX2, NEON, SVE, AMX) yetenek taraması (%100 dinamik CPUID/auxv/sysctl; statik liste yok).
  - Evrensel Dünya ISA Kataloğu (Intel Core i9 Extreme/Xeon-W çift 512b FMA, Fujitsu A64FX SVE512, Tenstorrent, Esperanto, Kalray, Sunway, s390x) ve kademeli fallback hiyerarşisi (`resolve_fallback`).
  - **SRAM / DRAM / CPU $\Leftrightarrow$ DRAM Hattı (Interconnect) & Çekirdek Topolojisi Keşfi:**
    - *SRAM Önbellek Hiyerarşisi*: L1i, L1d, L2, L3 boyutları, cacheline boyutu (64B / 128B Apple Silicon) ve set-associativity.
    - *DRAM Kapasitesi ve Sanal Sayfalama*: Toplam fiziksel RAM, kullanılabilir RAM, 4KB page size ve 2MB HugePages desteği.
    - *CPU $\Leftrightarrow$ DRAM Veri Yolu (Interconnect)*: NUMA node sayısı, bellek kanalları (Dual, Quad, Octa, UMA 512b/1024b), veri yolu genişliği (bit) ve teorik tepe bant genişliği (~GB/s).
    - *Çekirdek Topolojisi*: Fiziksel ve mantıksal çekirdekler, heterojen P-Core vs E-Core ayrımı (Intel 12th+, Snapdragon X Elite, Apple Firestorm/Icestorm).
    - *Dinamik Tiling & Streaming Store*: L1d bazlı optimal blok boyutu hesabı ve L3 aşımında otomatik streaming non-temporal store (`_mm256_stream_ps`).
    - *Anakart & DRAM Frekans Profili*: SMBIOS Type 2 (anakart modeli) ve Type 17 (DRAM hızı MT/s, XMP/EXPO/DOCP hız aşırtma profil tespiti).
    - *Anlık Saat & SFVM Upclocker (P-State Governor)*: QPC/RDTSC mikro-pencere ile anlık CPU GHz ölçümü, donanımsal HWP/CPPC frekans rampası için Anti-Downclock Warmup ve OS güç planı kilitleme (`-#SF|Upclocker:Allow/Max/Deny`).
  - Sıralı, paralel ve adaptif çoklu aday kernel eleme motoru.
  - Durgun kopya (Stagnant Snapshot) ile ana akışı kesmeden mikro-döngü (RDTSC) kıyaslaması.

- [x] **JVM-Tarzı CLI Bayrak Ayrıştırıcı & Dinamik Yapılandırma (`-#SF|Key:Value`):**
  - `MATHVANTAGE_OPTS` ve `SF_VM_ARGS` ortam değişkenleri ve API bayrak dizesi desteği.
  - `-#SF|GC_Mode:Cycles / SIMD` desteği.
  - `-#SF|Use_Critical_SIMDly_Operations:Allow / Deny / Auto` bayrağı ve mikro-SIMD bellek sıfırlama/canary doğrulama.
  - `GC_Mode:SIMD` ile `Use_Critical_SIMDly_Operations` arasındaki mimari çakışma (conflict) ve uyumsuzluk denetimi (`MSIMD_ERROR_FLAG_CONFLICT`).

- [x] **Sıfır Maliyetli Tensör I/O Durum Telemetrisi (Host $\Leftrightarrow$ VM Çift Yönlü İletişim):**
  - `SF_custominiVm_tensor_t` içindeki `_pad[3]` alanı `flags`, `status`, `_reserved` olarak değerlendirildi (%100 ABI ve boyut uyumlu, 0 ek bayt).
  - **Host $\rightarrow$ VM direktifleri**: `PINNED`, `IMMUTABLE`, `NO_PAGING`, `EPHEMERAL`.
  - **VM $\rightarrow$ Host bildirimleri**: `OK`, `FORWARDED` (Brooks ptr adresi güncellendi), `PAGED_IN`, `VIEW_SLICE`, `QUARANTINED` (Canary ihlali).
  - `SF_custominiVm_call` içinde otomatik taşınma ve karantina denetimi.

- [x] **Gelişmiş Deterministik GC Yürütücüsü & 3 Otomatik Heuristic Modu (Sfvm.Gc & OWP):**
  - **Tek ve Özgün Çağrı**: `Sfvm.Gc()` (aynı GC'nin yatağının yanına gidip kafasına su döküp uyandırma protokolü).
  - **Mütevazı Rapor (Manifesto)**: `Sfvm.GcReport()` ile temizlenen hücre, kazanılan MB, süpürülen kırıntılar ve derin uyku durumu.
  - **3 Otomatik GC Sezgisel (Heuristic) Modu (`-#SF|GC_Heuristic:Adaptive/Aggressive/Bureaucratic`)**:
    1. *`Adaptive` (Uyarlanabilir)*: Hafif ek CPU/RAM ile tahsisat hızını (velocity) ve mutator ritmini izler; sakin anlarda uyku aralığını 3-5 ms'ye uzatır, baskı altında 500 $\mu$s'ye indirir.
    2. *`Aggressive` (Saldırgan / Gözü Dönmüş)*: 250 $\mu$s minimum uyku; sağına soluna bakmadan boş kutuları böler, anında `VirtualFree(MEM_DECOMMIT)` ile OS'e iade eder, RAM ayak izini sıfır şişkinliğe kilitler.
    3. *`Bureaucratic` (Bürokratik / Tavizsiz Hijyenik)*: Çekilmez bir STW yanlısıdır. "Performans sürünse de zerre mikrop kapmamak" ilkesiyle her süpürmede bütün canlı hücrelerin tail canary ve magic değerlerini tek tek doğrular (`verifyTailCanary`), sıkı adres sıralı kompaktizasyon yapar.
  - `CONTINUNTIL.FINISH`: Heap tamamen temizlenene ve durağan hale gelene kadar otomatik döngü (`Sfvm.Gc(CONTINUNTIL.FINISH)`).
  - `STEPPING_MTP` (Manual Termination Point): Kullanıcı callback/lambda koşuluna bağlı adım adım yürütme.
  - `OWP` (Operation With Pauses): Mikro-duraklamalarla mutator'a nefes payı (`_mm_pause` / `yield`).
  - C API: `SF_custominiVm_gc_run()`, `SF_custominiVm_gc_collect()`, `SF_custominiVm_gc_set_heuristic()`, `SF_custominiVm_gc_get_heuristic()`.

- [x] **mathventagesimd: Evrensel Matematiksel Taksonomi ve Planck Ölçekli Ayrık Fizik Cephaneliği (Sections 28–39):**
  - **Kuramsal İkili Ontoloji**: Soyut Sürekli Analiz (Evren Dışı / İdealize Alan) $\Longleftrightarrow$ Planck Ölçeğinde Ayrık Evren Modeli ($\Delta x \ge \ell_P$, $\Delta t \ge t_P$, hücresel örgü).
  - **Section 28**: Planck Ölçekli Kuantalanmış Aritmetik ($X = n \cdot \ell_P$, $t = k \cdot t_P$, tam sayı 64-bit SIMD) ve Kahan-Babuška-Neumaier Telafili Toplam (`msimd_compensated_sum_neumaier`, `msimd_planck_discrete_step`).
  - **Section 29**: Transandantal Minimax & Özel Fonksiyonlar (Chebyshev/Remez Minimax Hata Fonksiyonu $\text{erf}(x)$, Lanczos $\Gamma(x)$, Legendre Polinomları $P_n(x)$, Estrin 8'li paralel FMA polinom açılımı).
  - **Section 30**: Winograd Minimal Filtreleme & Hızlı 2D Konvolüsyon ($F(2\times 2, 3\times 3)$ $U = G g G^T$, $V = B^T d B$, $Y = A^T M A$, kuramsal $2.25\times$ FMA hızlandırması).
  - **Section 31**: 3D Projektif Geometrik Cebir (3D PGA $\mathbb{R}_{3,0,1}^*$ Düzlem 1-blade $p$, Plücker Doğru 2-blade $l$, Nokta 3-blade trivector $P$, Motor 8-multivector $M$, Sandviç operatörü $X' = M X \widetilde{M}$, dış kesişim $\wedge$, regresif birleşim $\vee$).
  - **Section 32**: Çift Kuaterniyonlar (Dual Quaternions SE(3)) & Çift Lineer Harmanlama (DLB, $\hat{q} = q_r + \varepsilon q_d$, sıfır hacim kayıplı katı cisim interpolasyonu).
  - **Section 33**: Ayrık Dış Analiz (Discrete Exterior Calculus - DEC, üçgen örgü kotanjant Laplace-Beltrami ağırlıkları $\frac{1}{2 A_i} \sum (\cot \alpha + \cot \beta)$, 0-form ve 1-form ayrık Hodge yıldızı $\star$).
  - **Section 34**: Ayrık Varyasyonel Mekanik (Discrete Euler-Lagrange, Simplektik Euler $v_{k+1} = v_k + M^{-1} f \Delta t$, 2. mertebe Velocity Verlet, simplektik faz uzayı enerji korunumu denetleyicisi).
  - **Section 35**: Genişletilmiş Konum Tabanlı Dinamik (XPBD, uyumluluk $\alpha = 1/k$, mesafe kısıtı, 4-noktalı dörtyüzlü hacim kısıtı, temas ve çarpışma düzlemi stabilizasyonu).
  - **Section 36**: Hücresel & SPH Akışkanlar Dinamiği (Wendland $C^2$ kompakt çekirdek fonksiyonu $W(r,h)$ ve analitik gradyanı $\nabla W$, LBM D3Q19 Kafes Boltzmann BGK akış ve katı duvar geri sekme [bounce-back] adımı).
  - **Section 37**: Kuantum Durum Vektörü ve Üniter Kapı Dinamiği ($2^n$ karmaşık durum vektörü, Pauli-X, Pauli-Y, Pauli-Z, Hadamard, Faz $R_\phi(\theta)$, CNOT, Toffoli [CCNOT], bitmaskeli şerit permütasyonu ve ölçüm olasılıkları).
  - **Section 38**: Ayrık Uzay-Zaman, Regge Kalkülüsü ve Wilson Döngüleri (2D/4D simpleks eksik açıları [deficit angles], $U(1)$ ve $SU(2)$ Wilson plaquette eylemleri, 4D hiperkübik örgü Wilson-Dirac fermiyon şablonu).
  - **Section 39**: Sürekli Genel Görelilik & Jeodezik İzleme (Sözde-Riemann 4-boyutlu metrik tensörü $g_{\mu\nu}$, 2. tür Christoffel sembolleri $\Gamma^\mu_{\alpha\beta}$, 4-vektör jeodezik RK/Verlet entegratörü, Riemann eğrilik tensörü ve Ricci skaları).
  - **Section 40**: Sayısal Cebirsel Geometri & Homotopi Sürekliliği (MSC 14: Homotopi yolu izleme $H(x,t)=(1-t)G(x)+tF(x)$, Euler-Newton predictor-corrector, cebirsel yüzey tekillik ve Hessian analizi, Weierstrass eliptik eğri nokta toplama).
  - **Section 41**: Sistem Teorisi, Kalman Filtreleme & Optimal Kontrol (MSC 93: Lineer Kalman Filtresi durum ve kovaryans güncellemesi, Anti-Windup & filtreli türevli PID kontrolcüsü, Ayrık Zamanlı Cebirsel Riccati DARE LQR durum-geri beslemesi).
  - **Section 42**: Yöneylem Araştırması, Simplex & İç Nokta Yöntemleri (MSC 90: Simplex tablosu pivot operasyonu, en negatif indirgenmiş maliyet ve minimum oran testi, Dışbükey Kuadratik Programlama için Primal-Dual KKT İç Nokta bariyer adımı).
  - **Section 43**: Oyun Teorisi, Nash Dengesi & Minimax Stratejileri (MSC 91: 2-oyunculu bimatris oyunlar için Fictitious Play en iyi tepki dinamikleri, sıfır toplamlı oyun minimax değer hesabı).
  - **Section 44**: Biyomatematik & Popülasyon Dinamiği (MSC 92: 4. mertebe Runge-Kutta Lotka-Volterra av-avcı sistemi, SEIR salgın bölmeli diferansiyel modeli, Michaelis-Menten enzim kinetiği tepkime hızı).
  - **Section 45**: Topolojik Veri Analizi & Kalıcı Homoloji (MSC 55/68: Nokta bulutu $\ell_2$ metrik mesafe matrisi, $\mathbb{Z}_2$ sütun indirgemeli sınır matrisi $R = B \cdot V$, doğum-ölüm kalıcı homoloji barkod çiftleri).
  - **Section 46**: Çizge Algoritmaları & Ağ Akışı (MSC 68: En kısa yol Dijkstra, negatif ağırlıklı çevrim tespitli Bellman-Ford, artık çizge üzerinde Edmonds-Karp / Ford-Fulkerson maksimum akış).

- [x] **Temel Matematiksel ve Evrensel Fiziksel Sabitler Kataloğu (NIST / CODATA 2022+ & Planck Birimleri):**
  - `include/MathventageSIMD.h` içinde tek kaynak (Single-Source-of-Truth) olarak mühürlendi; C linkage (`extern "C"`) ve tüm harici bağlayıcılar (Java Panama FFM, C#, Python, Rust) ile uyumlu hale getirildi.
  - **Matematiksel Sabitler**: $\pi$, $2\pi$, $\pi/2$, $1/\pi$, $\sqrt{\pi}$, Euler sayısı $e$, Euler-Mascheroni sabiti $\gamma$, Altın Oran $\phi$, $\sqrt{2}$, $1/\sqrt{2}$, $\sqrt{3}$, $\ln 2$, $\ln 10$, $\varepsilon$.
  - **Evrensel Fiziksel Sabitler (NIST / CODATA SI)**: Işık hızı $c$, Planck sabiti $h$, İndirgenmiş Planck sabiti $\hbar$, Newton yerçekimi $G$, Boltzmann $k_B$, Avogadro $N_A$, Elementer yük $e$, Vakum geçirgenliği $\mu_0$, Dielektrik katsayısı $\varepsilon_0$, Elektron/Proton/Nötron kütleleri, İnce yapı sabiti $\alpha \approx 1/137$, Rydberg $R_\infty$, Stefan-Boltzmann $\sigma$.
  - **Planck Doğal Birimleri Kataloğu (Quantized Scale / Wikipedia & CODATA Standart)**: 
    - *Temel (Base)*: Planck uzunluğu $\ell_P$, zamanı $t_P$, kütlesi $m_P$, sıcaklığı $T_P$, yükü $q_P$.
    - *Türetilen (Derived)*: Planck alanı $A_P$, hacmi $V_P$, momentumu $p_P$, enerjisi $E_P$, kuvveti $F_P$, gücü $P_P$, yoğunluğu $\rho_P$, enerji yoğunluğu $\rho_{E,P}$, açısal frekansı $\omega_P$, basıncı $P_P$, ivmesi $a_P$, akımı $I_P$, gerilimi $V_P$, empedansı $Z_P$.

---


## 2. Yüksek Öncelik (High Priority)

- [x] **SIMD GC Motoru (Vektörel Çöp Toplayıcı Altyapısı):**
  - `-#SF|GC_Mode:SIMD` seçildiğinde devreye giren tam teşekküllü SIMD çöp toplayıcı motoru (`sweep_motherline_simd`).
  - AVX-512 / AVX2 / NEON ile 8'li ve 16'lı vektörel liveness bitmask taraması ve paralel ısı sönümleme (`_mm256_mul_ps` / `vmulq_f32`).
  - Sıfır dallanma maliyetli hızlı yol (fast-path branch-free skip) ve paralel durum maskesi çıkarımı.
  - SIMD vektörize sliding compaction (`compact_and_relocate_motherline_simd`): 32B/64B vektörel tensör transferi ve SIMD vektörel bellek sıfırlama ile anında Emptybox oluşturma.

- [x] **SBC (DIY Donanım), Sıfır-Syscall GPIO, Gerçek Zamanlı Sensör Tensör Akışı & Ring-0 Köprüsü:**
  - **SBC ve Gömülü Kart Keşfi (`SF_custominiVm_sbc_detect_board`)**:
    - Raspberry Pi 1 / Zero (BCM2835 MMIO: `0x20200000`)
    - Raspberry Pi 2 / 3 (BCM2836/2837 MMIO: `0x3F200000`, ARMv7/v8 NEON SIMD)
    - Raspberry Pi 4 (BCM2711 MMIO: `0xFE200000`)
    - Raspberry Pi 5 (RP1 Southbridge PCIe BAR, bcm2712 + rp1-peripherals.pdf)
    - LattePanda x86_64 (On-board ATmega32u4 / RP2040 Coprocessor DMA Bridge)
    - Nvidia Jetson (Tegra SoC MMIO + Unified RAM Zero-Copy)
    - Orange Pi (Allwinner H616 / Rockchip RK3588 MMIO)
  - 1 CPU Döngüsünde (~1-2ns) Sıfır-Syscall GPIO Okuma/Yazma (`SF_custominiVm_gpio_fast_write/read`).
  - **Gerçek Zamanlı Sürekli Sensör Tensör Akışı (Continuous Sensor Ingestion)**:
    - Sıcaklık/Nem, Su Seviyesi, IR Optik, IMU ve 32-kanal Mantık Analizörü (Logic Analyzer) sinyallerini doğrudan GC'den muaf (`PINNED`) SFVM tensör halka tamponuna (Ring-Buffer) akıtma.
    - GC Stop-the-World duraksamalarından %100 bağışık gerçek zamanlı matematiksel analiz.
  - **Ring-0 Kernel Köprüsü (`SFVM_K.sys` / `sfvm_k.ko`)**:
    - MSR HWP Turbo ve AVX-512 anti-downclocking taban frekans koruması.
    - Ring-3 `rdpmc` performansı için CR4.PCE yetkilendirmesi ve 1GB Giant Page fiziksel bellek eşleme temelleri.

- [x] **Çoklu Dil Binding Kütüphaneleri & Ana Uygulama Döngü Entegrasyonu:**
  - **Hedef Diller**: Java, C#, Rust, Python, C++.
  - **Temel Amaç ve Yetenekler**:
    1. *Bağımsız SFVM Başlatmadan Entegrasyon (Direct Embedded Host Loop)*: `SF_custominiVm_host_loop_tick(budget_us)` ile oyun motoru, fizik simülasyonu veya eğitim döngülerinin kendi frame adımları içinde sfGC'yi senkronize bir hızlandırıcı olarak koşturma.
    2. *Ana Uygulama GC'si ile Çift Yönlü Senkronizasyon (Bi-directional GC Sync)*: `SF_custominiVm_host_gc_notify(event_id)` ile Java (JVM ZGC/Shenandoah/G1), C# (.NET CoreCLR) ve Python bellek yöneticileri tetiklendiğinde sfGC'ye nefes aldırıp CPU/DRAM çekişmesini önleme.
    3. *sfGC Çalışırken Ana Uygulamaya Hookpoint Atama & Döngü Manipülasyonu*: `SF_custominiVm_register_gc_hook` ile `PRE_SWEEP`, `POST_SWEEP`, `PRE_COMPACT`, `POST_COMPACT`, ve `EMERGENCY_EVAC` evrelerinde ana uygulamaya mikro-saniyelik callback fırlatma; Brooks forwarding taşımaları öncesinde ana uygulama tensör referanslarını güvenceye alma (Pinning Guard).
    4. *Dile Özel Yüksek Başarımlı Köprüler*: Java FFM / Panama, C# unmanaged, Rust cxx, Python nanobind/DLPack, C++20 sfvm.

- [x] **SFVM İki Aşamalı Yaşam Döngüsü & Off-Heap Code Heap Anatomisi (`SFVM_Setup.Start()` & `SFVM_VM.Start()`):**
  - **1. Aşama (`SFVM_Setup.Start()`)**: On-Heap Runway Superblock, Falline, Landfill, VFS inode ağacı (`/sys/code_heap/rawcode_cell`, `/sys/code_heap/processed_chunk`, `/tensors`), SRAM/DRAM ve ISA keşfi.
  - **2. Aşama (`SFVM_VM.Start()`)**: Gömülü `Mathventory.bc.zstd` ikilisini açıp Rawcode Cell'e haritalama, `PAGE_READONLY` ile mıhlama (tüm çalışma zamanı boyunca kalıcı immutability).
  - **W^X Çift Tamponlu Bellek Güvenliği**: Processed Code Chunk staging alanında yazım, ardından atomik `VirtualProtect(PAGE_EXECUTE_READ)` ve `FlushInstructionCache()` ile mühürleme.
  - **Donanımsal Kademeli JIT (Tiered JIT Ladder)**: Tier 1 VAC $\rightarrow$ Tier 2 MIRC $\rightarrow$ Tier 3 Puzzle Arena ve asimetrik çekirdek/termal kısma deopt mekanizması.

- [x] **SacculusForge LLVM Bitcode JIT (ORCJIT) — Bitcode Üretiminde Otomatik Güvenlik & Safepoint Enjeksiyonu:**
  - LLVM IR pass (`SfvmSafepointPass`) ile her fonksiyon girişine (prologue) ve doğal döngü başlarına (`llvm::LoopInfo`) safepoint ekleme:
    - *Polling Safepoint (`sf_gc_safepoint_poll`)*: Uzun matematiksel döngülerde GC evresini (`DRAIN`/`SWEEP`) denetleyip işbirlikçi mikro-duraklama.
    - *Controlpoint (`sf_gc_controlpoint_check`)*: Yoğun döngü başında Runway bellek baskısı > %85 ise mutator tahsisatını frenleme (allocation pacing).
  - `CMakeLists.txt` üzerinden modern LLVM 22.1.5 (`orcjit`, `jitlink`, `passes`, `x86codegen`, `support`, `/MT` static embedding) otomatik tespiti ve `-DSACFOR_HAS_LLVM` entegrasyonu.

- [x] **LLVM TableGen Kural Kitabı & Otomatik SIMD Kernel Sentezi (`MathventageRules.td` & `codegen.py`):**
  - TableGen kural kitabı (`MathventageRules.td`) ile veri tipleri (`f32`, `f64`, `i32`, `i64`, `f16`, `bf16`), vektör ISA'ları (AVX-512, AVX2, SSE4.2, NEON, SVE, AMX, RVV, Scalar) ve tensör operasyon kurallarının tanımlanması.
  - `llvm-tblgen --dump-json` ile kural ağacının makine tarafından okunabilir JSON formatına dökülmesi.
  - `codegen.py --tablegen` ile JSON AST'den safepoint/controlpoint enjeksiyonlu, döngüleri açılmış (unrolled) 50+ C++ SIMD kernel'ının ve dispatch kayıt tablolarının otomatik türetilmesi (`src/sf_kernel_generated_rules.inl`).

---

## 3. Orta Öncelik (Medium Priority)

- [ ] **NUMA-Aware Room & Node Tahsisi:**
  - Her Room'un ve devasa tensörün donanımsal NUMA node'larından tahsis edilebilmesi.
  - `msimd_numa_topology_t` ile entegrasyon (uzak soket gecikmelerini sıfırlama).

- [ ] **Core-Specific & Asimetrik Sıcaklık Puanlaması (Per-Core Heat Score):**
  - Intel Hibrit mimarilerde (P-Core vs E-Core) ve ARM big.LITTLE yapılarında her çekirdeğin kendi sıcaklık skor listesini tutması.
  - P-core'da aşırı sıcak olan bir tensörün E-core'a geçtiğinde farklı soğuma eşikleriyle yönetilmesi.

- [ ] **Telemetri & Log Binary Dump / Replay:**
  - HotLog ring buffer'ının binary formatta diske akıtılması ve çevrimdışı profil çıkarma/oynatma.

- [x] **SFVM-SC (Safety Critical) — Khronos Vulkan SC Modeli ile Görev Kritik Standart & Gearkit:**
  - Tek dağıtım, tek ikili (Single Unified Binary) içinde çift profilli mimari:
    - **SFVM-Standard (`NORMAX`)**: Dinamik çok jenerasyonlu sfGC, JIT puzzle sandbox, sanal sayfa takası, dinamik runway genişlemesi, spekülatif optimizasyon.
    - **SFVM-SC (`SSIOLSC`)**:
      1. *Sıfır Dinamik Tahsisat*: Başlatma (Init) sonrası mutator/kernel döngüsünde `malloc`/`new`/`VirtualAlloc`/`mmap` kesinlikle yasak; $O(1)$ statik bump arena rezervasyonu (`msimd_context_alloc_aligned`).
      2. *Saf AOT & Sıfır JIT/GC Gecikmesi*: Uçuş/sürüş anında GC sliding compaction, background thread contention ve JIT de-opt jitter'ı yok; 742 kernel (503 dalga formu + 239 algoritma) AOT deterministik icra edilir.
      3. *Deterministik WCET (Worst-Case Execution Time)*: Tüm SIMD döngülerinde ve tensör boyutlarında katı üst sınır tavanı (`max_loop_iterations`); sonsuz döngü ve belirsiz yakınsama engellenir.
      4. *Bürokratik Sözleşme Denetimi (Contract Auditing)*: `SF_custominiVm_call` girişinde hizalama, geçersiz/null gösterici, NaN/Inf denetimi; hata durumunda çökme yerine anında `MSIMD_ERROR_INVALID_ARGUMENT` fail-safe dönüşü.
      5. *Zarif Gearkit Şanzıman Modeli*: `SacFor_Gearkit_NORMAX()` / `SacFor_Gearkit_SSIOLSC()` ve C++ `SF_Gearkit_Model::NORMAX()` / `SF_Gearkit_Model::SSIOLSC()` ile tek satırda deterministik mod geçişi. Bayrak veya argüman verilmezse güvenli varsayılan `NORMAX` olarak çalışır.
      6. *No-sfGC Overwrite-Heap Modu (Disk Depolama Sektörü Modeli)*:
         - **Felsefe**: Geleneksel dosya sistemlerinde (FAT/NTFS) bir dosya silindiğinde manyetik disk veya flaş hücreler fiziksel olarak sıfırlanmaz; sadece sektör tablosunda "boş" işaretlenir ve yeni veri geldiğinde eski baytların üzerine doğrudan yazılır (*In-Place Overwrite*). SFVM No-sfGC modu tam olarak bu modeli RAM'e uyarlar: `Allocate -> Clean -> Write` döngüsü yerine doğrudan `Direct Overwrite Heap` işletilir.
         - **Anatomik Dönüşüm (On-Heap & Off-Heap)**:
           - **Runway**: Dinamik chained-chunk zinciri donar; tek parça, sabit boyutlu statik monolitik sektöre dönüşür.
           - **Motherline**: Dinamik yaşlanma (Young/Adolescent/Senior) kuyrukları iptal edilir; sabit indeksli Statik İskandil Matrisi'ne (MFT/Inode tablosu) dönüşür.
           - **Falline**: Emptybox arama sepetleri yerine tek bitlik Reusable Sector Bitmap'e dönüşür. Ömrü biten hücre Falline'a taşınmaz, içeriği sıfırlanmaz; sadece üzerine yazılabilir (overwritable) damgası alır.
           - **Rooms (Young/Adol/Senior)**: Yaşam süresi odaları İPTAL. 3 Sabit Amaç Odasına dönüşür: *Room 0: Frame Scratch (her mikrosaniyede ezilen alan)*, *Room 1: Persistent Calibration State (sabit katsayılar)*, *Room 2: Hardware DMA Ring*.
           - **Regions (Demand/Middle/Tank/Hotfield)**: Dinamik Fibonacci ve OrderOfHeight ağaçları kaldırılır; önbellek satırına hizalı katı boyut partisyonlarına dönüşür (Küçük Vektör, Orta Tensör, Dev Tank, L1/L2 Pinned Hotfield).
           - **Landfill (Pit, Matured, Rehab, Pane)**: %100 DEVRE DIŞI / BYPASS. Çöp üretilmediği için hiçbir veri Landfill'e akıtılmaz; Landfill kilitlenir (0 Byte harcama).
           - **Code Heap (Rawcode Cell & Processed Code Chunk)**: Dinamik JIT staging tamamen yasaktır; tüm 742 kernel boot anında AOT derlenir ve `PAGE_EXECUTE_READ` ile çelik zırha alınır (W^X).
           - **HotLog & ColdLog**: GC telemetrisi susturulur; yerine 64B döngü/çerçeve bazlı Karakuşak Uçuş Kaydedici (Blackbox Flight Recorder: TSC, cycle time, NaN/Inf taşma kontrolü) devreye girer.
      7. *No-sfGC İkincil Çalışma Tipi — SCGC_Type (`DEFAULT` vs `PRIMITIVE_RW`)*:
         - **`DEFAULT`** (Phase-STW Bürokratik GC — SSIOLSC Seviye 1):
           - GC **yaşar**: faz aralarında sert STW + zorunlu kompaktizasyon devreye girer.
           - `sfLiveHeader` zinciri, `Emptybox` bitmap ve `Falline` kaydı korunur.
           - Geliştirici `SF_custominiVm_tensor_alloc()` ile hücre tahsisatı yapar.
           - Kullanım: Dinamik geçici tensörler üreten ama GC determinizmi istenen senaryolar (radar, EKG, nükleer kontrol).
         - **`PRIMITIVE_RW`** (Tam No-GC / Disk Sektörü — SSIOLSC Seviye 2):
           - GC **tamamen devre dışı**; hiçbir sweep, drain, compact veya STW duraklama olmaz.
           - `sfLiveHeader` hiç oluşturulmaz; saf ham bump pointer erişimi.
           - Geliştirici `SF_primitiva_alloc(bytes, align)` ile doğrudan Runway'den sektör alır.
           - `SF_primitiva_write/read/overwrite` ile bellek bariyerli (fence) ham R/W.
           - Disk mantığı: eski veri sıfırlanmaz, üzerine doğrudan yazılır.
           - Kullanım: 50 kHz ASML EUV DMA tamponları, lazer kontrol ring-buffer'ları, sabit katsayı ROM alanları.
         - **API**:
           ```c
           // C — Seviye 1 (STW GC yaşar)
           SacFor_Gearkit_SSIOLSC_Ex(SF_SCGC_TYPE_DEFAULT);

           // C — Seviye 2 (Tam No-GC, disk sektörü)
           SacFor_Gearkit_SSIOLSC_Ex(SF_SCGC_TYPE_PRIMITIVE_RW);
           void* buf = SF_primitiva_alloc(4096, 64);
           SF_primitiva_write(buf, &data, sizeof(data));
           SF_primitiva_read(buf, &out, sizeof(out));

           // C++
           SF_Gearkit_Model::SSIOLSC();       // DEFAULT: STW GC
           SF_Gearkit_Model::SSIOLSC(true);   // PRIMITIVE_RW: Tam No-GC
           void* buf = SF_Gearkit_Model::PrimitivaAlloc(4096);
           SF_Gearkit_Model::PrimitivaWrite(buf, &data, sizeof(data));

           // CLI
           -#SF|SCGC_Type:default       // Phase-STW STW GC
           -#SF|SCGC_Type:PrimitiveRW   // Tam No-GC, disk sektörü
           ```

---

## 4. Düşük Öncelik & Gelecek Vizyon (Research & Ecosystem)

- [ ] **Bepuphysics2 Hızlandırma Backend'i (`Bepuphysics2.SFVM`):**
  - C# tarafındaki Constraint Solver ve Narrowphase Collision aşamalarının MathventageSIMD'ye devredilmesi.
  - Standart Bepuphysics2 ile SFVM hızlandırmalı kıyaslama (Benchmark Suite).

- [ ] **WebAssembly (WASM) & SIMD128 Hedefi:**
  - Tarayıcı içi yüksek başarımlı tensör hesaplamaları için WASM backend araştırması.

- [ ] **ARM SVE2 & SME Donanımsal Matris Çarpım Desteği.**

- [ ] **NPU Hızlandırma Katmanı (Qualcomm Hexagon, Intel NPU, AMD XDNA, Apple ANE):**
  - Özellikle **Arduino VENTUNO Q** üzerindeki **Qualcomm Hexagon™ NPU (40 dense TOPS)** gibi uç birim yapay zeka hızlandırıcılarının `SF_Backend_Accelerated_Dispatcher.cpp` üzerinden devreye alınması (Qualcomm QNN SDK / OpenVINO NPU eklentisi).
  - Yüksek hacimli lineer cebir (MSC 15-XX) ve derin tensör daralmalarının NPU'ya yönlendirilmesi; analitik ve aşkın dalga fonksiyonlarının (MSC 33-XX, 34-XX) 8 çekirdekli Kryo CPU SIMD üzerinde sıfır veri yolu gecikmesiyle eşzamanlı işletilmesi.

- [ ] **HLSL / Compute Shader Grafik Hızlandırma Katmanı (Roadmap'in En Sonu):**
  - Saf CPU SIMD, DSP/Coprocessor ve NPU hızlandırma hedefleri tamamen olgunlaştıktan sonra, yol haritasının en son fazı olarak DirectCompute / HLSL ve Vulkan SPIR-V compute shader köprüsü tasarlanacaktır. Saf CPU determinizmi ve sıfır PCIe veri aktarım gecikmesi önceliğini korumak amacıyla GPU katmanı nihai sınır olarak konumlandırılmıştır.

---

## 5. Resmi Mimari ve Donanım Dökümanları Arşivi (Official Architecture Manuals)

Tüm dünya ISA ve donanım kataloğumuza ait resmi üretici kılavuzları `source_docs/` dizininde arşivlenmiştir:

| No | Mimari / Platform | Dosya Adı | Boyut | Açıklama |
|---|---|---|---|---|
| 1 | **Intel x86/x64** | `325462-sdm-vol-1-2abcd-3abcd-4.pdf` | 24.00 MB | Intel 64 & IA-32 SDM Combined |
| 2 | **AMD x86/x64** | `24593_3.44_APM_Vol2.pdf` | 11.98 MB | AMD64 Architecture Programmer's Manual Vol 2 |
| 3 | **RISC-V** | `riscv-spec.pdf` | 5.25 MB | RISC-V International 2026 Ratified Unprivileged + Privileged + RVV |
| 4 | **IBM Mainframe** | `zArch_SA22-7832.pdf` | 13.93 MB | IBM z/Architecture Principles of Operation SA22-7832 |
| 5 | **OpenPOWER** | `PowerISA_v3.1c.pdf` | 6.13 MB | OpenPOWER Foundation Power ISA v3.1c, POWER10 MMA & VSX |
| 6 | **LoongArch** | `LoongArch-Vol1-Basic-v1.10-EN.pdf` | 9.24 MB | Loongson Basic Architecture |
| 7 | **LoongArch SIMD** | `LoongArch-Vol2-Vector-v1.00-EN.pdf` | 22 KB | Loongson Vector Extensions LSX & LASX |
| 8 | **Broadcom / RPi4** | `bcm2711-peripherals.pdf` | 1.27 MB | Raspberry Pi Ltd Official BCM2711 ARM Peripherals |
| 9 | **RPi5 I/O Hub** | `rp1-peripherals.pdf` | 3.35 MB | Raspberry Pi Ltd Official RP1 Peripheral Architecture |
| 10 | **ARM AArch64** | `Arm_AAPCS64.pdf` | 385 KB | ARM-Software Official Procedure Call Standard AArch64 |
| 11 | **ARM SIMD ABI** | `Arm_Vector_Function_ABI_vfabia64.pdf` | 210 KB | ARM-Software Vector Calling Convention |
