#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 6/30/2011

#include "module/annotcloud/alias.h"
#include "module/annotcloud/annotation.h"
#include "module/mrlresolver/mrlinfo.h"
#include "module/searchengine/searchenginefactory.h"
#ifdef USE_MODE_SIGNAL
#  include "processinfo.h"
#endif // USE_MODE_SIGNAL
#include <QtGui/QMainWindow>
#include <QtCore/QMutex>
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QFileInfoList>

QT_BEGIN_NAMESPACE
class QGestureEvent;
class QMimeData;
class QNetworkAccessManager;
class QNetworkCookieJar;
class QPanGesture;
class QPinchGesture;
class QSwipeGesture;
class QTimer;
class QUrl;
QT_END_NAMESPACE

namespace QtExt {
  class CountdownTimer;
  class MouseRubberBand;
  class CircularRubberBand;
} // namespace QtExt

// Objects
class AnnotationFilter;
class ClientAgent;
class ClipboardMonitor;
class Database;
class DataManager;
class DataServer;
class EventLogger;
class FadeAnimation;
class Grabber;
class Magnifier;
class MrlResolver;
class Player;
class RippleImageFilter;
class ServerAgent;
class SignalHub;
class Tray;
class Translator;

class AcPlayerServer;
class AcDownloader;
class AcBrowser;

// Views
class AnnotationGraphicsView;
class AnnotationAnalyticsView;
class AnnotationBrowser;
class AnnotationEditor;
//class CommentView;
class MainPlayerUi;
class MessageHandler;
class MiniPlayerUi;
//class MiniPlayerDock;
//class OsdDock;
class OsdWindow;
class OsdConsole;
class EmbeddedPlayerUi;
class EmbeddedCanvas;
class PlayerUi;
class Preferences;
class MessageView;
class SignalView;
class BacklogDialog;
class ConsoleDialog;
class TokenView;
class UserAnalyticsView;
class VideoView;

// Dialogs
class AnnotationCountDialog;
class BlacklistView;
class DeviceDialog;
class HelpDialog;
class LiveDialog;
class LoginDialog;
//class NetworkProxyDialog;
class PickDialog;
class SeekDialog;
//class SiteAccountView;
class MediaInfoView;
class SyncDialog;
class UserView;
class UrlDialog;

// MainWindow
class MainWindow : public QMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)
  typedef MainWindow Self;
  typedef QMainWindow Base;

  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;
  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  // - Types -
public:
  enum Color {
    DefaultColor = 0,
    White, Cyan, Blue, Red, Purple, Orange, Black
  };

  // - Constructions -
public:
  explicit MainWindow(bool unique = true, QWidget *parent = 0, Qt::WindowFlags f = 0);
  bool isValid() const;

public:
  void move(const QPoint &pos) { Base::move(pos); emit posChanged(); }
  void move(int x, int y) { Base::move(x, y); emit posChanged(); }

  // - Signals -
signals:
  void windowPicked(WId);
  void userIdChanged(qint64 uid);
  void posChanged();
  void openAnnotationUrlRequested(const QString &url);
  void annotationUrlAdded(QString url);
  void windowMaximized();
  void playingFinished();
  void playMediaRequested();
  void trayToolTip(const QString &tip);
  void trayMessage(const QString &title, const QString &message);
  void response(const QString &text);
  void said(const QString &text, const QString &color);
  void message(const QString &text);
  void errorMessage(const QString &text);
  void warning(const QString &text);
  void notification(const QString &text);
  void showTextRequested(const QString &text);
  void windowClosed();
  void windowTitleToChange(QString title);
  void downloadProgressUpdated();
  void downloadFinished();
  void progressMessageChanged(const QString &text);

  void seeked();
  void addAndShowAnnotationRequested(const Annotation &a);
  void addAnnotationsRequested(const AnnotationList &l);
  void showAnnotationRequested(const Annotation &a);
  void showAnnotationOnceRequested(const Annotation &a);
  void setAnnotationsRequested(const AnnotationList &l);
  void appendAnnotationsRequested(const AnnotationList &l);

  void userChanged(User user);

  // - States -
public:
  bool isPlaying() const;
  bool isEditing() const;

  bool isWindowOnTop() const;

  bool hasVisiblePlayer() const;
  PlayerUi *visiblePlayer(); // One of three player Uis, or nullptr.

  bool inputLineHasFocus() const;
  bool prefixLineHasFocus() const;

  bool isTranslateEnabled() const;
  bool isSubtitleOnTop() const;

  bool isAutoPlayNext() const;
  bool repeatCurrent() const;
  bool noRepeat() const;

public slots:
  void setAutoPlayNext();
  void setRepeatCurrent();
  void setNoRepeat();

  void maximizedToFullScreen();

  void setTranslateEnabled(bool enabled);
  void translate(const QString &text);
  void translate(const QString &text, int lang);

  void setWindowOnTop(bool t = true);
  void toggleWindowOnTop()
  { setWindowOnTop(!isWindowOnTop()); }

  //void ensureWindowOnTop();

  void setSubtitleOnTop(bool t);
  void setSubtitleColorToDefault();
  void setSubtitleColorToWhite();
  void setSubtitleColorToCyan();
  void setSubtitleColorToBlue();
  void setSubtitleColorToRed();
  void setSubtitleColorToPurple();
  void setSubtitleColorToOrange();
  void setSubtitleColorToBlack();
  int subtitleColor() const;
  void setSubtitleColor(int colorId);
private:
  void uncheckSubtitleColorActions();

public slots:
  void onFocusedWidgetChanged(QWidget *w_old, QWidget *w_new);

  void setEmbeddedWindow(WId winId);

  void about();
  //void quit();
  void help();
  void update();

  void login(const QString &userName, const QString &encryptedPassword, bool async = true);
  void logout(bool async = true);

  void checkInternetConnection(bool async = true);
  void deleteCaches();

  void newWindow();

  void openSource(const QString &source);
  void openSources(const QStringList &l);
  void open();  ///< By default the same as openFile()
  void openFile();
  void openUrl();
  void openAnnotationUrl();
  void openAnnotationUrlFromAliases(const AliasList &l, bool async = true);
  void openAnnotationUrl(const QString &url, bool save = false);
  void openUrl(const QString &url);
  void openUrl(const QString &url, bool save);
  void openProcess();
  void openWindow();
  void openDevice();
  void openDevicePath(const QString &path, bool isAudioCD);
  void openAudioDevice();
  void openVideoDevice();
  void openSubtitle();
  void openDirectory();
  void openMrl(const QString &path, bool checkPath = true);
  void openStreamUrl(const QString &rtsp);
  void openRemoteMedia(const MediaInfo &mi, QNetworkCookieJar *jar = 0);
  void openLocalUrl(const QUrl &url);
  void openLocalUrls(const QList<QUrl> &urls);
  void openMimeData(const QMimeData *urls);
  void openSubtitleFile(const QString &path);
  void closeMedia();
  void play();
  void playPause();
  void replay(); /// Restart from the beginning.
  void pause();
  void stop();
  void nextFrame();
  void snapshot(bool mediaOnly = true);
  void snapshotAll() { snapshot(false); }

  void previous();
  void next();
  void autoPlayNext();

  void seek(qint64 msecs);
  void forward(qint64 msecs = 0);  ///< If <=0, default delta time is used.
  void backward(qint64 msecs = 0); ///< If <=0, default delta time is used.

  void forward5s()      { forward(5 * 1000); }
  void backward5s()     { backward(5 * 1000); }
  void forward10s()     { forward(10 * 1000); }
  void backward10s()    { backward(10 * 1000); }
  void forward25s()     { forward(25 * 1000); }
  void forward30s()     { forward(30 * 1000); }
  void backward30s()    { backward(30 * 1000); }
  void forward60s()     { forward(60 * 1000); }
  void backward60s()    { backward(60 * 1000); }
  void forward85s()     { forward(85 * 1000); }
  void forward90s()     { forward(90 * 1000); }
  void backward90s()    { backward(90 * 1000); }
  void forward1m()      { forward(1 * 60000); }
  void backward1m()     { backward(1 * 60000); }
  void forward5m()      { forward(5 * 60000); }
  void backward5m()     { backward(5 * 60000); }
  void forward10m()     { forward(10 * 60000); }
  void backward10m()    { backward(10 * 60000); }

  void volumeUp(qreal delta = 0);  ///< If <=0, default delta percent is used.
  void volumeDown(qreal delta = 0);///< If <=0, default delta percent is used.

  //void about(); // TODO
  //void help(); // TODO

  void updateWindowTitle();
  void updateDownloadProgress(qint64 receivedBytes, qint64 totalBytes);
  void syncInputLineText(const QString &text);
  void syncPrefixLineText(const QString &text);

  void enableWindowTransparency();
  void disableWindowTransparency();

  void showVideoViewIfAvailable();

  void setMultipleWindowsEnabled(bool t);

  void openInWebBrowser();
  void downloadCurrentUrl();
  void copyCurrentUrl();
  void copyCurrentTitle();
  void clickProgressButton();
protected:
  bool hasNext() const;
  QString newWindowTitle() const;
  QString currentUrl() const;
  QString currentTitle() const;
  static QString downloadSpeedToString(int speed);

public slots:
  void invalidateMediaAndPlay(bool async = true);
protected slots:
  void playMedia();
  void updateWindowSize();

  void updateTraceButtons();

  void setRippleEnabled(bool t);
  void enableRipple() { setRippleEnabled(true); }
  void disableRipple() { setRippleEnabled(false); }

  // - Live -
public slots:
  void openChannel();
  void closeChannel();
  void updateLiveAnnotations(bool async = true);

  // - Cursor -
//protected:
//  bool isCursorVisible() const { return cursorVisible_; }
//protected slots:
//  void setCursorVisible(bool t);
//  void showCursor() { setCursorVisible(true); }
//  void hideCursor() { setCursorVisible(false); }
//  void resetAutoHideCursor();
//  void autoHideCursor();

  // - Video adjustment -
protected slots:
  void contrastUp(); void contrastDown(); void contrastReset();
  void brightnessUp(); void brightnessDown(); void brightnessReset();
  void hueUp(); void hueDown(); void hueReset();
  void saturationUp(); void saturationDown(); void saturationReset();
  void gammaUp(); void gammaDown(); void gammaReset();

  // - Annotation transformations -
protected slots:
  void annotationScaleUp();
  void annotationScaleDown();
  void annotationRotateUp();
  void annotationRotateDown();
  void increaseAnnotationOffset();
  void decreaseAnnotationOffset();

  // - Dialogs -
protected:
  UrlDialog *annotationUrlDialog();
  UrlDialog *mediaUrlDialog();
  AnnotationAnalyticsView *annotationAnalyticsView();

protected slots:
  void showUserAnalytics(qint64 userId);

public slots:
  void showLoginDialog();
  void hideLoginDialog();
  void setLoginDialogVisible(bool visible);

  void setMagnifierVisible(bool visible);
  void showMagnifier() { setMagnifierVisible(true); }
  void hideMagnifier() { setMagnifierVisible(true); }
  void toggleMagnifierVisible();

  //void setNetworkProxyDialogVisible(bool visible);

  void setUserViewVisible(bool visible);

  void setAnnotationEditorVisible(bool visible);
  void toggleAnnotationEditorVisible();

  void setBlacklistViewVisible(bool visible);
  void showBlacklistView() { setBlacklistViewVisible(true); }

  void setTokenViewVisible(bool t);
  void toggleTokenViewVisible();

  void setAnnotationBrowserVisible(bool t);
  void toggleAnnotationBrowserVisible();

  void setAnnotationAnalyticsViewVisible(bool visible);
  void toggleAnnotationAnalyticsViewVisible();
  void showAnnotationAnalyticsView() { setAnnotationAnalyticsViewVisible(true); }

  void openHomePage();

  void showSeekDialog();
  void hideSeekDialog();
  void setSeekDialogVisible(bool visible);

  void setAnnotationCountDialogVisible(bool visible);

  void showLiveDialog();
  void hideLiveDialog();
  void setLiveDialogVisible(bool visible);

  void showSyncDialog();
  void hideSyncDialog();
  void setSyncDialogVisible(bool visible);

  //void showCommentView();
  //void hideCommentView();
  //void setCommentViewVisible(bool visible);

  void showWindowPickDialog() { setWindowPickDialogVisible(true); }
  void setWindowPickDialogVisible(bool visible);

  void showProcessPickDialog() { setProcessPickDialogVisible(true); }
  void setProcessPickDialogVisible(bool visible);

  void setBacklogDialogVisible(bool visible);
  void showBacklogDialog() { setBacklogDialogVisible(true); }

  void setConsoleDialogVisible(bool visible);

  void showDownloader();

  void setMediaInfoViewVisible(bool visible);
  void showMediaInfoView() { setMediaInfoViewVisible(true); }

  //void setSiteAccountViewVisible(bool visible);
protected slots:
  //void invalidateSiteAccounts();
  BacklogDialog *backlogDialog();
  ConsoleDialog *consoleDialog();

  // - User -
public slots:
  void setUserAnonymous(bool t, bool async = true);
  void setUserLanguage(int lang, bool async = true);

  // - Languages -
protected slots:
  void setAppLanguageToEnglish();
  void setAppLanguageToJapanese();
  void setAppLanguageToTraditionalChinese();
  void setAppLanguageToSimplifiedChinese();
  void invalidateAppLanguage();

  void setUserLanguageToEnglish();
  void setUserLanguageToJapanese();
  void setUserLanguageToChinese();
  void setUserLanguageToKorean();
  void setUserLanguageToUnknown();

  void invalidateAnnotationLanguages();

  // - Error handling -
protected slots:
  void handlePlayerError();

  // - Annotations -
public slots:
  void eval(const QString &cmd);
  void chat(const QString &text, bool async = true);
  void respond(const QString &text);
  void say(const QString &text, const QString &color = "blue");
  void warn(const QString &text);
  void showMessage(const QString &text);
  void notify(const QString &text);
  void error(const QString &text);

  void submitText(const QString &text, bool async = true);
  void showText(const QString &text, bool isSigned = false);

  void submitLiveText(const QString &text, bool async = true);

  void showTextAsSubtitle(const QString &text, bool isSigned = false);

  void updateAnnotations(bool async = true);
  void setToken(const QString &filePath = QString(), bool async = true);
  void invalidateToken(const QString &mrl = QString());
  void submitAlias(const Alias &alias, bool async = true);
  void submitAliasText(const QString &text, qint32 type = 0, bool async = true);

  void signFile(const QString &path, bool async = true);
  void signFileWithUrl(const QString &path, const QString &url, bool async = true);

  void blessTokenWithId(qint64 tid, bool async = true);
  void curseTokenWithId(qint64 tid, bool async = true);

  void blessUserWithId(qint64 tid, bool async = true);
  void curseUserWithId(qint64 tid, bool async = true);
  void blockUserWithId(qint64 tid, bool async = true);

  void blessAliasWithId(qint64 tid, bool async = true);
  void curseAliasWithId(qint64 tid, bool async = true);
  void blockAliasWithId(qint64 tid, bool async = true);

  void blessAnnotationWithId(qint64 tid, bool async = true);
  void curseAnnotationWithId(qint64 tid, bool async = true);
  void blockAnnotationWithId(qint64 tid, bool async = true);

  // - Remote annotations -
public slots:
  void importUrl(const QString &url);
  void importAnnotationsFromUrl(const QString &suburl);
  bool isAnnotationUrlRegistered(const QString &suburl) const;
protected slots:
  bool registerAnnotationUrl(const QString &suburl);
  void addRemoteAnnotations(const AnnotationList &l, const QString &url = QString());
  void clearAnnotationUrls();

  // - Recent -
protected slots:
  void addRecent(const QString &path);
  void openRecent(int i);
  void clearRecent();
  void updateRecent();
  void updateRecentMenu();

  // - Playlist -
protected:
  int currentPlaylistIndex() const;
protected slots:
  void setRecentOpenedFile(const QString &path);
  void openPlaylistItem(int i);
  void openNextPlaylistItem();
  void openPreviousPlaylistItem();
  void clearPlaylist();
  void updatePlaylist();
  void updatePlaylistMenu();

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
  void dispose();
public:
  virtual bool event(QEvent *event); ///< \override
  void gestureEvent(QGestureEvent *e);
  void panGesture(QPanGesture *g);
  void pinchGesture(QPinchGesture *g);
  void swipeGesture(QSwipeGesture *g);
protected:
  virtual void paintEvent(QPaintEvent *event);
  virtual void mousePressEvent(QMouseEvent *event); ///< \override
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \override
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< \override
  virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< \override
  virtual void wheelEvent(QWheelEvent *event); ///< \override

  virtual void moveEvent(QMoveEvent *event); ///< \override
  virtual void resizeEvent(QResizeEvent *event); ///< \override

  virtual void changeEvent(QEvent *event); ///< \override

  virtual void keyPressEvent(QKeyEvent *event); ///< \override
  virtual void keyReleaseEvent(QKeyEvent *event); ///< \override

  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

  virtual void closeEvent(QCloseEvent *event); ///< \override

  virtual void focusInEvent(QFocusEvent *e); ///< \override
  virtual void focusOutEvent(QFocusEvent *e); ///< \override

protected slots:
  virtual void dragEnterEvent(QDragEnterEvent *event); ///< \override
  virtual void dragMoveEvent(QDragMoveEvent *event); ///< \override
  virtual void dragLeaveEvent(QDragLeaveEvent *event); ///< \override
  virtual void dropEvent(QDropEvent *event); ///< \override

  void updateAnnotationHoverGesture();

  void updateContextMenu();
  void updateGameMenu();
  void updateAspectRatioMenu();
  void updateSettingsMenu();
  void updateMenuTheme();
  void updateAnnotationSettingsMenu();
  void updateAnnotationSubtitleMenu();
  void updateUserMenu();
  void updateTrackMenu();

  void rememberPlayPos();
  void resumePlayPos();

  void rememberSubtitle();
  void resumeSubtitle();

  void rememberAudioTrack();
  void resumeAudioTrack();

  void rememberAspectRatio();
  void resumeAspectRatio();

  void resumeAll();

  void loadSubtitles();
  void loadSubtitlesLater();

protected:
  static bool canResume(const QString &url);

protected:
  bool isGlobalPosNearEmbeddedPlayer(const QPoint &pos) const;
  bool isGlobalPosOverEmbeddedPositionSlider(const QPoint &pos) const;
  bool isGlobalPosOverOsdConsole(const QPoint &pos) const;

public slots:
  void updatePlayMode();
  void updatePlayerMode();
  void updateTokenMode();
  void updateWindowMode();
  void updateOsdWindowOnTop();

  void invalidatePlayerMode();

  void resumeAnnotations();

  // - Themes -
public slots:
  void setThemeToDefault();
  void setThemeToRandom();
  void setThemeToDark();
  void setThemeToBlack();
  void setThemeToBlue();
  void setThemeToBrown();
  void setThemeToCyan();
  void setThemeToGray();
  void setThemeToGreen();
  void setThemeToPink();
  void setThemeToPurple();
  void setThemeToRed();
  void setThemeToWhite();
  void setThemeToYellow();

  // - Annotation effect -
protected slots:
  void updateAnnotationEffectMenu();
  void setAnnotationEffectToDefault();
  void setAnnotationEffectToTransparent();
  void setAnnotationEffectToShadow();
  void setAnnotationEffectToBlur();

  // - Navigation -
protected:
  bool isNavigable() const;
protected slots:
  void setPreviousEnabled(bool t);
  void setNextEnabled(bool t);
  void setNavigationEnabled(bool t);
  void enableNavigation() { setNavigationEnabled(true); }
  void disableNavigation() { setNavigationEnabled(false); }

  void openPreviousMrl();
  void openNextMrl();

  // - Browse -
public slots:
  void setBrowsedFile(const QString &fileName);
  void setBrowsedUrl(const QString &mrl);
  void openBrowsedDirectory();
  void openBrowsedFile(int id);
  void moreBrowsedFiles();
  void lessBrowsedFiles();
  void openPreviousFile();
  void openNextFile();

  void showMinimizedAndPause();
  void actualSize();
protected:
  void updateBrowseMenu(const QString &fileName);
  void updateBrowseMenu();
  int currentBrowsedFileId() const;

  // - Proxy browser -
protected slots:
  void openProxyBrowser();

  // - Clipboard -
protected slots:
  void enterAnnotationUrl(const QString &url);
  void enterMediaUrl(const QString &url);
  void enterDownloadUrls(const QStringList &urls);

  // - Close menu -
protected slots:
  void nothingAfterFinished();
  void sleepAfterFinished();
  void shutdownAfterFinished();
  void promptSleep();
  void promptShutdown();

  // - Signal mode -
#ifdef WITH_WIN_QTH
signals:
  void attached(ProcessInfo pi);
  void detached(ProcessInfo pi);
public slots:
  void openProcessPath(const QString &path);
  void openProcessHook(ulong hHook, const ProcessInfo &pi = ProcessInfo());
  void openProcessWindow(WId hwnd);
  void openProcessId(ulong pid);
#endif // WITH_WIN_QTH

public slots:
  void setWindowTrackingEnabled(bool t);

#ifdef USE_MODE_SIGNAL
public slots:
  void showSignalView();
  void hideSignalView();
  void setSignalViewVisible(bool visible);

  void showRecentMessageView();
  void hideRecentMessageView();
  void setRecentMessageViewVisible(bool visible);

private:
  SignalView *signalView_;
  MessageView *recentMessageView_;
  MessageHandler *messageHandler_;

  QAction *toggleSignalViewVisibleAct_,
          *toggleRecentMessageViewVisibleAct_;
#endif // USE_MODE_SIGNAL

  // - Helpers -
protected:
  static bool isMimeDataSupported(const QMimeData *urls);
  static bool isDevicePath(const QString &path);
  static QString removeUrlPrefix(const QString &url);

  static QString digestFromFile(const QString &path);

  static QString languageToString(int lang);
  static int fileType(const QString &fileName);

  static QString shortenText(const QString &text);

  static bool isRemoteMrl(const QString &mrl);

  bool isPosOutOfScreen(const QPoint &globalPos) const;
  bool isRectOutOfScreen(const QRect &globalRect) const;

  bool isDigestReady(const QString &path) const;
  bool isAliasReady(const QString &path) const;

  // - Search -
public slots:
  void searchWithEngine(int engine, const QString &key);
protected slots:
  void searchCurrentTitleWithGoogle() { searchWithEngine(SearchEngineFactory::Google, currentTitle()); }
  void searchCurrentTitleWithGoogleImages() { searchWithEngine(SearchEngineFactory::GoogleImages, currentTitle()); }
  void searchCurrentTitleWithBing()   { searchWithEngine(SearchEngineFactory::Bing, currentTitle()); }
  void searchCurrentTitleWithNicovideo() { searchWithEngine(SearchEngineFactory::Nicovideo, currentTitle()); }
  void searchCurrentTitleWithBilibili() { searchWithEngine(SearchEngineFactory::Bilibili, currentTitle()); }
  void searchCurrentTitleWithAcfun()  { searchWithEngine(SearchEngineFactory::Acfun, currentTitle()); }
  void searchCurrentTitleWithYoutube() { searchWithEngine(SearchEngineFactory::Youtube, currentTitle()); }
  void searchCurrentTitleWithYouku()  { searchWithEngine(SearchEngineFactory::Youku, currentTitle()); }
  void searchCurrentTitleWithWikiEn() { searchWithEngine(SearchEngineFactory::WikiEn, currentTitle()); }
  void searchCurrentTitleWithWikiJa() { searchWithEngine(SearchEngineFactory::WikiJa, currentTitle()); }
  void searchCurrentTitleWithWikiZh() { searchWithEngine(SearchEngineFactory::WikiZh, currentTitle()); }

  // - Dialogs -
protected slots:
  void showPreferences();

  // - Rubber band -
protected slots:
  void pauseAnnotationsAt(const QRect &rect);
  void resumeAnnotationsAt(const QRect &rect);
  void removeAnnotationsAt(const QRect &rect);

protected slots:
  void setDefaultAspectRatio();
  void setWideScreenAspectRatio(); // 16:9
  void setStandardAspectRatio(); // 4:3
  void updateAnnotationMetaVisible();

protected slots:
  void setSubmit(bool t) { submit_ = t; }
  void setSaveMedia(bool t);
  void saveMedia();

  // - Members for initialization. -
private:
  void setupWindowStyle();
  void resetPlayer();
  void createComponents(bool unique);
  void createActions();
  void createMenus();
  void createSearchMenu();
  void createSearchEngines();
  void createDockWindows();
  void createConnections();

  // - Attributes -
private:
  bool disposed_;
  bool submit_;
  int liveInterval_; // TO BE REMOVED
  mutable QMutex inetMutex_;    // mutext for remote communication
  mutable QMutex annotMutex_;
  Preferences *preferences_;
  Tray *tray_;
  SignalHub *hub_;

  Magnifier *magnifier_;

  QList<SearchEngine *> searchEngines_;

  QtExt::CountdownTimer *loadSubtitlesTimer_;
  QtExt::CountdownTimer *resumePlayTimer_,
                        *resumeSubtitleTimer_,
                        *resumeAudioTrackTimer_,
                        *resumeAspectRatioTimer_;

  QTimer *liveTimer_;
  QTimer *windowStaysOnTopTimer_;
  QTimer *navigationTimer_;
  //QTimer *autoHideCursorTimer_;
  QTimer *holdTimer_;
  QStringList annotationUrls_;

  //QTimer *windowStaysOnTopTimer_;

  QStringList recentFiles_;
  QHash<QString,QString> recentUrlTitles_;
  QString browsedUrl_;
  QFileInfoList browsedFiles_;

  Translator *translator_;

  AcPlayerServer *appServer_;
  AcBrowser *browserDelegate_;
  AcDownloader *downloaderDelegate_;

#ifdef WITH_MODULE_SERVERAGENT
  ServerAgent *server_;
#endif // WITH_MODULE_SERVERAGENT
#ifdef WITH_MODULE_CLIENTAGENT
  ClientAgent *client_;
#endif // WITH_MODULE_CLIENTAGENT

  Player *player_;

  Database *cache_,
           *queue_;
  DataManager *dataManager_;
  DataServer *dataServer_;

  ClipboardMonitor *clipboardMonitor_;
  MrlResolver *mrlResolver_;

  EventLogger *logger_;
  OsdConsole *globalOsdConsole_;

  //OsdDock *osdDock_;
  OsdWindow *osdWindow_;
  TokenView *tokenView_;
  VideoView *videoView_;

  MainPlayerUi *mainPlayer_;
  MiniPlayerUi *miniPlayer_;
  EmbeddedPlayerUi *embeddedPlayer_;
  EmbeddedCanvas *embeddedCanvas_;

  AnnotationGraphicsView *annotationView_;
  AnnotationAnalyticsView *annotationAnalyticsView_;
  UserAnalyticsView *userAnalyticsView_;
  AnnotationBrowser *annotationBrowser_;
  AnnotationEditor *annotationEditor_;
  AnnotationFilter *annotationFilter_;

  QtExt::MouseRubberBand *pauseRubberBand_,
                         *resumeRubberBand_,
                         *removeRubberBand_;
  QtExt::CircularRubberBand *attractRubberBand_,
                            *expelRubberBand_;

  BlacklistView *blacklistView_;
  //CommentView *commentView_;
  BacklogDialog *backlogDialog_;
  ConsoleDialog *consoleDialog_;

  AnnotationCountDialog *annotationCountDialog_;
  DeviceDialog *deviceDialog_;
  HelpDialog *helpDialog_;
  LoginDialog *loginDialog_;
  LiveDialog *liveDialog_;
  //NetworkProxyDialog *networkProxyDialog_;
  PickDialog *processPickDialog_;
  SeekDialog *seekDialog_;
  SyncDialog *syncDialog_;
  PickDialog *windowPickDialog_;
  UrlDialog *mediaUrlDialog_;
  UrlDialog *annotationUrlDialog_;
  //SiteAccountView *siteAccountView_;
  MediaInfoView *mediaInfoView_;
  UserView *userView_;

  QPoint dragPos_,
         pressedPos_,
         movedPos_;
  Qt::MouseButtons pressedButtons_;
  qint64 windowModeUpdateTime_;

  qint32 tokenType_; // current token type tt

  Grabber *grabber_;

  QString recentOpenedFile_;
  QString recentPath_;
  QString recentDigest_;
  QString recentSource_;

  bool recentSourceLocked_;

  //bool cursorVisible_;

  bool cancelContextMenu_;

  bool rippleEnabled_;
  RippleImageFilter *rippleFilter_;
  QTimer *rippleTimer_;

  QStringList playlist_;

  QHash<qint64, qint64> playPosHistory_;
  QHash<qint64, int> subtitleHistory_;
  QHash<qint64, int> audioTrackHistory_;
  QHash<qint64, QString> aspectRatioHistory_;

  int preferredSubtitleTrack_,
      preferredAudioTrack_;

  FadeAnimation *fadeAni_;

  QWidgetList windows_;

  // - Menus and actions -
  QMenu *contextMenu_,
        *currentMenu_,
        *searchMenu_,
        *advancedMenu_,
        *helpContextMenu_,
        *recentMenu_,
        *userMenu_,
        *openMenu_,
        *openButtonMenu_,
        *playMenu_,
        *subtitleMenu_,
        *audioTrackMenu_,
        *browseMenu_,
        *trackMenu_,
        *sectionMenu_,
        *backwardMenu_,
        *forwardMenu_,
        *adjustMenu_,
        *appLanguageMenu_,
        *annotationEffectMenu_,
        *userLanguageMenu_,
        *settingsMenu_,
        *utilityMenu_,
        *closeMenu_,
        *annotationLanguageMenu_,
        *themeMenu_,
        *playlistMenu_,
        *subtitleStyleMenu_,
        *annotationSubtitleMenu_;
  QList<QAction*> contextMenuActions_;

  QMenu *annotationMenu_;
  QMenu *annotationSettingsMenu_;
  QAction *resumeAnnotationAct_,
          *increaseAnnotationScaleAct_,
          *decreaseAnnotationScaleAct_,
          *resetAnnotationScaleAct_,
          *increaseAnnotationOffsetAct_,
          *decreaseAnnotationOffsetAct_,
          *resetAnnotationOffsetAct_,
          *increaseAnnotationRotationAct_,
          *decreaseAnnotationRotationAct_,
          *resetAnnotationRotationAct_;

  QAction *contrastUpAct_, *contrastDownAct_, *contrastResetAct_,
          *hueUpAct_, *hueDownAct_, *hueResetAct_,
          *gammaUpAct_, *gammaDownAct_, *gammaResetAct_,
          *saturationUpAct_, *saturationDownAct_, *saturationResetAct_,
          *brightnessUpAct_, *brightnessDownAct_, *brightnessResetAct_;

  QAction *previousSectionAct_,
          *nextSectionAct_;
  QAction *previousFileAct_,
          *nextFileAct_,
          *moreFilesAct_,
          *lessFilesAct_,
          *openBrowsedDirectoryAct_;

  QAction *checkInternetConnectionAct_;
  QAction *deleteCachesAct_;

  QAction *setSubtitleColorToDefaultAct_,
          *setSubtitleColorToWhiteAct_,
          *setSubtitleColorToCyanAct_,
          *setSubtitleColorToBlueAct_,
          *setSubtitleColorToRedAct_,
          *setSubtitleColorToPurpleAct_,
          *setSubtitleColorToOrangeAct_,
          *setSubtitleColorToBlackAct_;

  QMenu *aspectRatioMenu_;
  QAction *setDefaultAspectRatioAct_,
          *setStandardAspectRatioAct_,
          *setWideScreenAspectRatioAct_;

  QAction *setAnnotationEffectToDefaultAct_,
          *setAnnotationEffectToTransparentAct_,
          *setAnnotationEffectToShadowAct_,
          *setAnnotationEffectToBlurAct_;

  QAction *newWindowAct_;

  QAction *openAct_,
          *openFileAct_,
          *openUrlAct_,
          *openAnnotationUrlAct_,
          *openDeviceAct_,
          *openVideoDeviceAct_,
          *openAudioDeviceAct_,
          *openSubtitleAct_,
          *playAct_,
          *pauseAct_,
          *stopAct_,
          *replayAct_,
          *nextFrameAct_,
          *previousAct_,
          *nextAct_,
          *snapshotAct_,
          *snapshotAllAct_,
          *actualSizeAct_,
          *toggleAnnotationAnalyticsViewVisibleAct_,
          *toggleAnnotationBandwidthLimitedAct_,
          *toggleAnnotationVisibleAct_,
          *toggleAnnotationAvatarVisibleAct_,
          *toggleAnnotationCountDialogVisibleAct_,
          *toggleMagnifierVisibleAct_,
          *toggleMenuBarVisibleAct_,
          *toggleClipboardMonitorEnabledAct_,
          //*toggleSiteAccountViewVisibleAct_,
          *toggleFullScreenModeAct_,
          *toggleEmbeddedModeAct_,
          *toggleMiniModeAct_,
          *toggleLiveModeAct_,
          *toggleSyncModeAct_,
          *toggleSubtitleVisibleAct_,
          *toggleSubtitleAnnotationVisibleAct_,
          *toggleNonSubtitleAnnotationVisibleAct_,
          *toggleTranslateAct_,
          *toggleSubtitleOnTopAct_,
          *toggleEmbeddedPlayerOnTopAct_,
          *toggleAutoPlayNextAct_,
          *toggleRepeatCurrentAct_,
          *toggleNoRepeatAct_;

  QAction *nothingAfterFinishedAct_,
          *sleepAfterFinishedAct_,
          *shutdownAfterFinishedAct_;

  QAction *toggleWindowOnTopAct_;
  QAction *togglePlayerLabelEnabled_;
  QAction *downloadCurrentUrlAct_,
          *copyCurrentUrlAct_,
          *copyCurrentTitleAct_,
          *openInWebBrowserAct_,
          *openHomePageAct_;

  QAction *updateAnnotationsAct_;

  QAction *loginAct_,
          *logoutAct_,
          *connectAct_,
          *disconnectAct_;

  QAction *clearCacheAct_;

  QMenu   *gameMenu_;
  QAction *toggleAnnotationBrowserVisibleAct_,
          *toggleAnnotationEditorVisibleAct_,
          *toggleTokenViewVisibleAct_,
          *toggleLiveDialogVisibleAct_,
          *toggleLoginDialogVisibleAct_,
          //*toggleNetworkProxyDialogVisibleAct_,
          *showWindowPickDialogAct_,
          *showProcessDialogAct_,
          *toggleSeekDialogVisibleAct_,
          *toggleSyncDialogVisibleAct_,
          *toggleMediaInfoViewVisibleAct_,
          *toggleUserViewVisibleAct_,
          *toggleBlacklistViewVisibleAct_,
          *toggleBacklogDialogVisibleAct_,
          *toggleDownloaderVisibleAct_,
          *toggleConsoleDialogVisibleAct_,
          *toggleAnnotationFilterEnabledAct_;
          //*toggleCommentViewVisibleAct_,
  QAction *toggleWindowTrackingAct_;
  WId lastTrackedWindow_;

  QAction *toggleUserAnonymousAct_;
  QAction *openProxyBrowserAct_;

  QAction *forward5sAct_,
          *backward5sAct_,
          *forward10sAct_,
          *backward10sAct_,
          *forward25sAct_,
          *backward30sAct_,
          *forward60sAct_,
          *backward60sAct_,
          *forward85sAct_,
          *backward90sAct_,
          *forward1mAct_,
          *backward1mAct_,
          *forward5mAct_,
          *backward5mAct_,
          *forward10mAct_,
          *backward10mAct_;

  QAction *clearRecentAct_;
  QAction *clearPlaylistAct_,
          *nextPlaylistItemAct_,
          *previousPlaylistItemAct_;

  QAction *setAppLanguageToEnglishAct_,
          *setAppLanguageToJapaneseAct_,
          *setAppLanguageToTraditionalChineseAct_,
          *setAppLanguageToSimplifiedChineseAct_;

  QAction *setUserLanguageToEnglishAct_,
          *setUserLanguageToJapaneseAct_,
          *setUserLanguageToChineseAct_,
          *setUserLanguageToKoreanAct_,
          *setUserLanguageToUnknownAct_;

  QAction *toggleAnnotationLanguageToEnglishAct_,
          *toggleAnnotationLanguageToJapaneseAct_,
          *toggleAnnotationLanguageToChineseAct_,
          *toggleAnnotationLanguageToKoreanAct_,
          *toggleAnnotationLanguageToUnknownAct_,
          *toggleAnnotationLanguageToAnyAct_;

  QAction *toggleAeroEnabledAct_,
          *toggleMenuThemeEnabledAct_;

  QAction *toggleMultipleWindowsEnabledAct_;
  QAction *toggleSubmitAct_;
  QAction *toggleSaveMediaAct_,
          *saveMediaAct_,
          *togglePreferLocalDatabaseAct_;

  QAction *setThemeToDefaultAct_,
          *setThemeToRandomAct_,
          *setThemeToDarkAct_,
          *setThemeToBlackAct_,
          *setThemeToBlueAct_,
          *setThemeToBrownAct_,
          *setThemeToCyanAct_,
          *setThemeToGrayAct_,
          *setThemeToGreenAct_,
          *setThemeToPinkAct_,
          *setThemeToPurpleAct_,
          *setThemeToRedAct_,
          *setThemeToWhiteAct_,
          *setThemeToYellowAct_;


  QAction *showMaximizedAct_,
          *showMinimizedAct_,
          *showNormalAct_,
          *helpAct_,
          *updateAct_,
          *aboutAct_,
          *preferencesAct_,
          *quitAct_;
};

#endif // MAINWINDOW_H
