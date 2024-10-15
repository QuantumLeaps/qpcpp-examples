//============================================================================
// Product: DPP example, GTK3+-GUI
// Last updated for version: 7.3.0
// Date of the Last Update:  2023-08-21
//
//                   Q u a n t u m  L e a P s
//                   ------------------------
//                   Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <www.gnu.org/licenses/>.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include <thread>

#include "qpcpp.hpp"
#include "safe_std.h" // portable "safe" <stdio.h>/<string.h> facilities
#include "bsp.hpp"
#include "dpp.hpp"

#include <glibmm.h>
#include <gtkmm.h>
#include <gdk/gdk.h>

Q_DEFINE_THIS_FILE

extern "C" {
//............................................................................
// Prototypes
void assert_failed(char const * const module, int_t const id);
static gpointer appThread(gpointer task_data);

} // extern "C"

//............................................................................
using namespace Gtk;
using namespace Gdk;
using namespace Glib;

//............................................................................
// the main application window class
class DppWindow : public Gtk::Window
{
    public:
        DppWindow();
        virtual ~DppWindow();
        void notify(ustring message,  ustring caption, bool wantToQuit = false);
        enum mtxState {
            UNLOCK = 0,
            LOCK,
            TRY_LOCK
        };
        bool doMutex(enum mtxState s);

    protected:
        // signal handlers
        void buttonClicked();
        bool on_close_request(GdkEventAny *evt);

        // local methods
        gint messageBox(ustring text, ustring caption, bool yesNo = false);
        bool eventHandler();
        void copy();

        // local variables -----------------------------------------------------------
        Grid grid;
        Entry texts[2];
        Image philos[APP::N_PHILO];

        Label smLabel;
        Label empty = Label("");

        Button btnCopy = Button("Copy");
        Button btnPlay;
        Button btnQuit;

        ButtonBox btnBox;
        ButtonBox playBox;
        ButtonBox quitBox;
        Box editBox;

        gint l_rnd;
        bool run = true;
        bool requestQuit = false;

        // local constants -----------------------------------------------------------
        const Glib::ustring iconPlay  = "media-playback-start";
        const Glib:: ustring iconPause = "media-playback-pause";
        const Glib::ustring iconQuit  = "application-exit";
        const Glib::ustring philoPics[3] = {
            "/dpp/example/pics/think",
            "/dpp/example/pics/hungry",
            "/dpp/example/pics/eat"
        };
        const guint8 philoPos[APP::N_PHILO][2] = {
            {2, 0}, {4, 1}, {3, 2}, {1, 2}, {0, 1}
        };

    private:
        Glib::ustring message;
        Glib::ustring caption;
        bool notification = false;
};

//............................................................................
#ifdef Q_SPY
    enum {
        PHILO_STAT = QP::QS_USER
    };
    static const guint8 l_clock_tick = 0U;
#endif

typedef enum picIdx {
    UNDEF = -1,
    THINK = 0,
    HUNGRY,
    EAT
} picIdx_t;

static gchar *l_cmdLine = NULL;
static Glib::RefPtr<Gtk::Application> l_app;
static Glib::Mutex myMutex;
static std::thread tlThread;
static DppWindow *l_hwnd;
static picIdx_t philoState[APP::N_PHILO];
static bool noSpy = false;

//............................................................................
DppWindow::~DppWindow()
{
}

//............................................................................
DppWindow::DppWindow()
{
    l_hwnd = this;

    // init the philo state array
    for(gint n = 0; n < APP::N_PHILO; n++)
    {
        philoState[n] = UNDEF;
    }

    set_title(ustring::sprintf("DPP Example (GTKmm %d.%d)", GTKMM_MAJOR_VERSION, GTKMM_MINOR_VERSION));
    set_icon(Gdk::Pixbuf::create_from_resource("/dpp/example/icons/qp"));
    set_resizable(false);
    set_default_size (420, 250);
    set_position(WIN_POS_CENTER);

    smLabel.set_markup("<span foreground=\"blue\" size=\"smaller\"><i>https://www.state-machine.com</i></span>");

    btnQuit.set_image_from_icon_name(iconQuit);
    btnQuit.set_always_show_image(true);

    btnPlay.set_image_from_icon_name(iconPause);
    btnPlay.set_always_show_image(true);

    grid.set_column_homogeneous(true);
    grid.set_column_spacing(4);
    grid.set_row_spacing(4);

    for(int i = 0; i < 2; i++)
    {
        texts[i].set_text(ustring::sprintf("Edit %1d...", i + 1));
    }

    editBox.set_homogeneous(false);
    editBox.pack_start(texts[0], true, true, 0);
    editBox.pack_start(btnCopy, false, true, 0);
    editBox.pack_start(texts[1], true, true, 0);

    for(gint i = 0; i < APP::N_PHILO; i++)
    {
        philos[i].set_from_resource(philoPics[HUNGRY]);
        grid.attach(philos[i], philoPos[i][0], philoPos[i][1]);
    }

    playBox.set_layout(BUTTONBOX_CENTER);
    playBox.pack_start(btnPlay, true, false, 10);

    quitBox.set_layout(BUTTONBOX_END);
    quitBox.pack_end(btnQuit, true, false, 10);

    grid.attach(playBox, 1, 1, 3, 1);
    // grid.attach(empty, 0, 3, 5, 1);
    grid.attach(editBox, 0, 4, 5, 1);
    grid.attach(quitBox, 3, 0, 2, 1);
    grid.attach(smLabel, 0, 5, 5, 1);

    this->signal_delete_event().connect(sigc::mem_fun(*this, &DppWindow::on_close_request));
    btnQuit.signal_clicked().connect(sigc::mem_fun(*this, &DppWindow::close));
    btnCopy.signal_clicked().connect(sigc::mem_fun(*this, &DppWindow::copy));
    btnPlay.signal_clicked().connect(sigc::mem_fun(*this, &DppWindow::buttonClicked));

    add(grid);
    gdk_threads_add_idle((GSourceFunc)&DppWindow::eventHandler, this);
    show_all();
}

//............................................................................
gint DppWindow::messageBox(ustring text, ustring caption, bool yesNo)
{
    const ustring iconQuestion = "dialog-question-symbolic";
    const ustring iconInfo = "dialog-information-symbolic";
    Dialog mb(caption, true);
    Box *content = mb.get_content_area();
    Label message(text);
    Image icon;
    icon.set_from_icon_name(ustring(yesNo ? iconQuestion : iconInfo), IconSize(ICON_SIZE_DIALOG));

    mb.set_resizable(false);
    mb.set_keep_above(true);
    mb.set_position(WIN_POS_CENTER);
    content->add(icon);
    content->add(message);
    mb.add_button(ustring(yesNo ? "_Yes" : "_OK"), GTK_RESPONSE_OK);
    if(yesNo)
    {
        mb.add_button(ustring("_No"), GTK_RESPONSE_NO);
    }
    mb.set_size_request(250, -1);

    mb.show_all();
    gint result = mb.run();
    mb.~Dialog();

    return result;
}

void DppWindow::notify(ustring message,  ustring caption, bool wantToQuit)
{
    this->message = message;
    this->caption = caption;
    this->requestQuit = wantToQuit;
    this->notification = true;
}

//............................................................................
bool DppWindow::eventHandler()
{
    static bool firstRun = true;
    bool retCode = true;

    if(firstRun)
    {
        firstRun = false;
        myMutex.unlock(); // was locked in 'main()'
    }

    if(myMutex.trylock())
    {
        if(notification) {
            notification = false;
            messageBox(message, caption);
            message.clear();
            caption.clear();
        }
        else for(gint n = 0; n < APP::N_PHILO; n++)
        {
            picIdx_t l = philoState[n];
            if(l != UNDEF)
            {
                philos[n].set_from_resource(philoPics[l]);
                philoState[n] = UNDEF;
            }
        }
        if(requestQuit)
        {
            requestQuit = false;
            if(! in_destruction())
                this->~DppWindow();
            retCode = false;
        }
        myMutex.unlock();
    }
    return retCode;
}

//............................................................................
void DppWindow::buttonClicked()
{
    static const QP::QEvt pe[2] = {
        QP::QEvt(APP::PAUSE_SIG),
        QP::QEvt(APP::SERVE_SIG)
    };
    gint peIdx;

    myMutex.lock();
    peIdx = (run ? 0 : 1);
    btnPlay.set_image_from_icon_name(run ? iconPlay : iconPause);
    run = !run;
    myMutex.unlock();

    APP::AO_Table->POST(&pe[peIdx], nullptr);
}

//............................................................................
void DppWindow::copy()
{
    myMutex.lock();
    texts[1].set_text(texts[0].get_text());
    myMutex.unlock();
}

//............................................................................
bool DppWindow::on_close_request(GdkEventAny *evt)
{
    notify("\n\n           Bye bye!\nSee you again next time...\n\n", "Good bye", true);
    return false;
}

//............................................................................
#if defined main
#undef main
#endif
int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        l_cmdLine = g_strdup(argv[1]);
        argc = 1;
        argv[1] = NULL;
    }

    myMutex.lock();

    tlThread = std::thread(main_gui);
    tlThread.detach();

    l_app = Application::create(argc, argv, "com.state-machine.example");
    l_hwnd = new DppWindow();

    gint status = l_app->run(*l_hwnd);
    BSP::terminate(0);

    return status;
}

//............................................................................
namespace BSP {
//............................................................................
void init(int argc, char **argv) {
    (void)argc;
    (void)argv;

    myMutex.lock(); // block thread until main window is running
    if (!QS_INIT(l_cmdLine)) { // QS initialization failed?
        l_hwnd->notify("Cannot connect to QSPY via TCP/IP\n"
                "Please make sure that 'qspy -t' is running",
                "QS_INIT() Error", true);
    }
    myMutex.unlock();

    // send the QS dictionaries...
    QS_OBJ_DICTIONARY(&l_clock_tick);
    QS_USR_DICTIONARY(PHILO_STAT);

    // setup the QS filters...
    QS_GLB_FILTER(QP::QS_ALL_RECORDS);
    QS_GLB_FILTER(-QP::QS_QF_TICK);
}
//............................................................................
void start(void) {
    // initialize event pools
    static QF_MPOOL_EL(APP::TableEvt) smlPoolSto[2*APP::N_PHILO];
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe
    static QP::QSubscrList subscrSto[APP::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // start AOs/threads...

    static QP::QEvtPtr philoQueueSto[APP::N_PHILO][10];
    for (std::uint8_t n = 0U; n < APP::N_PHILO; ++n) {
        APP::AO_Philo[n]->start(
            n + 3U,                  // QP prio. of the AO
            philoQueueSto[n],        // event queue storage
            Q_DIM(philoQueueSto[n]), // queue length [events]
            nullptr, 0U);            // no stack storage
    }

    static QP::QEvtPtr tableQueueSto[APP::N_PHILO];
    APP::AO_Table->start(
        APP::N_PHILO + 7U,       // QP prio. of the AO
        tableQueueSto,           // event queue storage
        Q_DIM(tableQueueSto),    // queue length [events]
        nullptr, 0U);            // no stack storage
}
//............................................................................
void terminate(gint16 result) {
    QP::QF::stop(); // stop the main QF application

    // cleanup all QWIN resources...

    // end the main dialog

}
//............................................................................
void displayPhilStat(uint8_t n, char const *stat)
{
    picIdx_t bitmapNum = THINK;

    Q_REQUIRE(n < APP::N_PHILO);

    switch (stat[0]) {
        case 't': bitmapNum = THINK; break;
        case 'h': bitmapNum = HUNGRY; break;
        case 'e': bitmapNum = EAT; break;
        default: Q_ERROR();  break;
    }
    // set the "segment" # n to the bitmap # 'bitmapNum'
    // gtk_image_set_from_resource(GTK_IMAGE(philos[n]), philoPics[bitmapNum]);
    myMutex.lock();
    philoState[n] = bitmapNum;
    myMutex.unlock();

    QS_BEGIN_ID(PHILO_STAT, APP::AO_Philo[n]->getPrio()) // app-specific record
        QS_U8(1, n);  // Philosopher number
        QS_STR(stat); // Philosopher status
    QS_END()
}
//............................................................................
void displayPaused(guint8 paused)
{
    // already done in button callback
}
//............................................................................
static Glib::Rand myRnd;

std::uint32_t random(void) {  // a very cheap pseudo-random-number generator
    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    //
    return myRnd.get_int() >> 8;
}
//............................................................................
void randomSeed(guint32 seed) {
    myRnd.set_seed(seed);
}

} // namespace BSP

//............................................................................
namespace QP {
//............................................................................
void  QF::onStartup(void) {
    setTickRate(BSP::TICKS_PER_SEC, 30); // set the desired tick rate
}
//............................................................................
void QF::onCleanup(void) {
    tlThread.~thread();
}
//............................................................................
void QF::onClockTick(void) {
    QTimeEvt::TICK_X(0U, &l_clock_tick); // process time events at rate 0

    QS_RX_INPUT(); // handle the QS-RX input
    QS_OUTPUT();   // handle the QS output
}

//----------------------------------------------------------------------------
#ifdef Q_SPY // define QS callbacks

//............................................................................
//! callback function to execute user commands
void QS::onCommand(uint8_t cmdId, uint32_t param1,
                   uint32_t param2, uint32_t param3)
{
    Q_UNUSED_PAR(cmdId);
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);
}

#endif // Q_SPY
//----------------------------------------------------------------------------

} // namespace QP

extern "C" {
//............................................................................
Q_NORETURN Q_onError(char const * const module, int_t const id) {
    char message[80];
    QP::QF::stop(); // stop ticking

    QS_ASSERTION(module, id, 10000U); // report assertion to QS
    myMutex.lock();
    SNPRINTF_S(message, Q_DIM(message) - 1U,
               "Assertion failed in module %s location %d", module, id);
    l_hwnd->notify(ustring(message), ustring("!!! ASSERTION !!!"), true);
    myMutex.unlock();

    QP::QF::onCleanup();
    QS_EXIT();
}

//............................................................................
void assert_failed(char const * const module, int_t const id)
{
    Q_onError(module, id);
}

} // extern "C"
