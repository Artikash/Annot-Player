// eventlogger.cc
// 8/7/2011

#include "eventlogger.h"
#include "mainwindow.h"
#include "global.h"
#include "tr.h"
#include "signalhub.h"
#include "application.h"
#include "project/common/acui.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/datetime.h"
#include "module/player/player.h"
#include <QtGui/QApplication>
#include <QtCore>

#define DEBUG "eventlogger"
#include "module/debug/debug.h"

EventLogger::EventLogger(MainWindow *logger, Player *player, SignalHub *hub, QObject *parent)
  : Base(parent), logger_(logger), player_(player), hub_(hub), logCount_(0)
{
  Q_ASSERT(player_);

  logUntilPlayingTimer_ = new QTimer(this);
  logUntilPlayingTimer_->setInterval(G_LOGGER_PLAYING_WAITER_TIMEOUT);

  createConnections();
}

void
EventLogger::createConnections()
{
  connect(player_, SIGNAL(buffering()), SLOT(startLogUntilPlaying()), Qt::QueuedConnection);
  connect(player_, SIGNAL(stopped()), SLOT(stopLogUntilPlaying()), Qt::QueuedConnection);

  //connect(player_, SIGNAL(titleIdChanged(int)), SLOT(logTitleChanged()));
  connect(player_, SIGNAL(mediaChanged()), SLOT(logMediaChanged()), Qt::QueuedConnection);
  connect(player_, SIGNAL(mediaClosed()), SLOT(logMediaClosed()), Qt::QueuedConnection);
  connect(player_, SIGNAL(volumeChanged()), SLOT(logVolumeChanged()), Qt::QueuedConnection);
  connect(player_, SIGNAL(subtitleChanged()), SLOT(logSubtitleChanged()), Qt::QueuedConnection);
  connect(player_, SIGNAL(audioTrackChanged()), SLOT(logAudioTrackChanged()), Qt::QueuedConnection);
  connect(player_, SIGNAL(opening()), SLOT(logOpening()), Qt::QueuedConnection);
  connect(player_, SIGNAL(buffering()), SLOT(logBuffering()), Qt::QueuedConnection);
  connect(player_, SIGNAL(playing()), SLOT(logPlaying()), Qt::QueuedConnection);
  connect(player_, SIGNAL(paused()), SLOT(logPaused()), Qt::QueuedConnection);
  connect(player_, SIGNAL(stopped()), SLOT(logStopped()), Qt::QueuedConnection);
  connect(player_, SIGNAL(errorEncountered()), SLOT(logPlayerError()), Qt::QueuedConnection);
  connect(player_, SIGNAL(errorEncountered()), SLOT(stopLogUntilPlaying()), Qt::QueuedConnection);
  connect(player_, SIGNAL(trackNumberChanged(int)), SLOT(logTrackNumberChanged(int)), Qt::QueuedConnection);
  connect(player_, SIGNAL(rateChanged(qreal)), SLOT(logPlayRateChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(aspectRatioChanged(QString)), SLOT(logAspectRatioChanged(QString)), Qt::QueuedConnection);
  connect(player_, SIGNAL(contrastChanged(qreal)), SLOT(logContrastChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(brightnessChanged(qreal)), SLOT(logBrightnessChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(hueChanged(int)), SLOT(logHueChanged(int)), Qt::QueuedConnection);
  connect(player_, SIGNAL(saturationChanged(qreal)), SLOT(logSaturationChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(gammaChanged(qreal)), SLOT(logGammaChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(audioChannelChanged(int)), SLOT(logAudioChannelChanged(int)), Qt::QueuedConnection);
  connect(player_, SIGNAL(audioDelayChanged(qint64)), SLOT(logAudioDelayChanged(qint64)), Qt::QueuedConnection);

  connect(AcUi::globalInstance(), SIGNAL(aeroEnabledChanged(bool)), SLOT(logAeroEnabledChanged(bool)));
  connect(AcUi::globalInstance(), SIGNAL(menuEnabledChanged(bool)), SLOT(logMenuEnabledChanged(bool)));

  connect(logUntilPlayingTimer_, SIGNAL(timeout()), SLOT(logUntilPlaying()));
}

// - Logging -

void
EventLogger::startLogUntilPlaying()
{
  //DOUT("enter");
  if (player_->hasMedia()) {
    logCount_ = 0;
    if (!logUntilPlayingTimer_->isActive()) {
      Application::globalInstance()->setCursor(Qt::BusyCursor);
      logUntilPlayingTimer_->start();
    }
  }
  //DOUT("exit");
}

void
EventLogger::stopLogUntilPlaying()
{
  DOUT("enter");
  Q_ASSERT(logUntilPlayingTimer_);
  if (logUntilPlayingTimer_ && logUntilPlayingTimer_->isActive()) {
    Application::globalInstance()->setCursor(Qt::ArrowCursor);
    logUntilPlayingTimer_->stop();
  }
  DOUT("exit");
}

void
EventLogger::logUntilPlaying()
{
  if (!player_->hasMedia())
    return;

  if (player_->isPlaying() || player_->isPaused()) {
    stopLogUntilPlaying();
    // FIXME: to be moved to other player_!!
    if (player_->isMouseEventEnabled())
      player_->startVoutTimer();
    return;
  }

  if (!logCount_)
    logger_->notify(tr("caching media or fonts ... this could take up to 10min on first launch, don't panic!"));
  else {
    int msecs = logCount_ * G_LOGGER_PLAYING_WAITER_TIMEOUT;
    QTime t = QtExt::msecs2time(msecs);
    QString s = QString("%1 / %2").arg(t.toString("m:ss"));
    if (logCount_ < 100) // 5sec * 100 < 10:0min = 60sec * 10
        s = s.arg("10:00");
    else
        s = s.arg("15:00");
    logger_->notifyOnce(tr("Patient ... ") + s);
  }
  logCount_++;
}

void
EventLogger::logMediaChanged()
{ logger_->showMessage(tr("media changed")); }

void
EventLogger::logTitleChanged()
{ logger_->showMessage(tr("title changed")); }

void
EventLogger::logMediaClosed()
{ logger_->showMessage(tr("media closed")); }

void
EventLogger::logVolumeChanged()
{
  //static bool first = true;
  //if (first) {
  //  first = false;
  //  return;
  //}

  if (player_->isValid()) {
    int v = player_->volume() * 100;
    if (v == 50)
      return;
    QString msg = QString("%1: " HTML_STYLE_OPEN(color:orange) "%2%" HTML_STYLE_CLOSE())
        .arg(TR(T_VOLUME)).arg(QString::number(v));
    logger_->showMessageOnce(msg);
  }
}

void
EventLogger::logSubtitleChanged()
{ logger_->showMessage(tr("subtitle changed")); }

void
EventLogger::logAudioTrackChanged()
{ logger_->showMessage(tr("audio track changed")); }

void
EventLogger::logOpening()
{ logger_->showMessageOnce(tr("Opening ...")); }

void
EventLogger::logBuffering()
{
  enum { interval = 2000 }; // 2 seconds
  static qint64 time = 0;
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (now <= time + interval) {
    time = now;
    return;
  }
  time = now;
  logger_->showMessageOnce(tr("Buffering ..."));
  //logger_->showMessage(tr("buffering ..."));
}

void
EventLogger::logPlaying()
{
  QString title;
  if (player_->hasMedia()) {
    title = player_->mediaTitle();
    if (title.isEmpty()) {
      title = player_->mediaPath();
      if (!title.isEmpty())
        title = QFileInfo(title).fileName();
    }
  }
  logger_->showMessageOnce(tr("Playing"));
  logger_->showMessage(tr("playing") + ": " + title);
}

void
EventLogger::logStopped()
{ logger_->showMessageOnce(tr("Stopped")); }

void
EventLogger::logPaused()
{ logger_->showMessageOnce(tr("Paused")); }

void
EventLogger::logPlayerError()
{ logger_->warnOnce(tr("Player Error")); }

void
EventLogger::logTrackNumberChanged(int track)
{ logger_->showMessage(tr("opening track %1").arg(QString::number(track))); }

void
EventLogger::logAudioChannelChanged(int ch)
{
  QString msg;
  switch (ch) {
  case Player::LeftChannel: msg = tr("left"); break;
  case Player::RightChannel: msg = tr("right"); break;
  case Player::StereoChannel: msg = tr("stereo"); break;
  case Player::ReverseStereoChannel: msg = tr("reverse stereo"); break;
  case Player::DolbysChannel: msg = tr("Dolby's"); break;
  case Player::NoChannel: msg = tr("none");
  default: ;
  }
  msg = HTML_STYLE_OPEN(color:orange) + msg + HTML_STYLE_CLOSE();
  msg.prepend(tr("audio channel") + ": ");
  logger_->showMessage(msg);
}

void
EventLogger::logPlayRateChanged(qreal rate)
{
  int r = qRound(rate);
  if (r == 1)
    logger_->showMessage(tr("resume playing"));
  else
    logger_->showMessage(tr("fast forward") +
      QString(": " HTML_STYLE_OPEN(color:orange) "x%1" HTML_STYLE_CLOSE())
      .arg(QString::number(r))
    );
}

// - Server -

void
EventLogger::logLoginRequested(const QString &userName)
{ logger_->showMessage(tr("logging in as %1 ...").arg(userName)); }

void
EventLogger::logLoginSucceeded(const QString &userName)
{ logger_->showMessage(tr("login succeeded as %1").arg(userName)); }

void
EventLogger::logLoginFailed(const QString &userName)
{ logger_->warn(tr("failed to login as %1").arg(userName)); }

void
EventLogger::logLogoutRequested()
{ logger_->showMessage(tr("logging out ...")); }

void
EventLogger::logLogoutFinished()
{ logger_->showMessage(tr("user logged out")); }

void
EventLogger::logSeeked(qint64 msecs)
{
  if (player_->isValid()) {
    qint64 len = player_->mediaLength();
    QTime t = QtExt::msecs2time(msecs);
    QTime l = QtExt::msecs2time(len);
    QString msg = QString("%1: " HTML_STYLE_OPEN(color:orange) "%2" HTML_STYLE_CLOSE() " / %3")
        .arg(tr("Seek"))
        .arg(t.toString())
        .arg(l.toString());
    if (len)
      msg += QString(" (" HTML_STYLE_OPEN(color:orange) "%1%" HTML_STYLE_CLOSE() ")")
             .arg(QString::number(msecs * 100.0 / len, 'f', 1));
    logger_->showMessageOnce(msg);
  }
}


void
EventLogger::logAudioDelayChanged(qint64 msecs)
{
  QTime t = QtExt::msecs2time(msecs);
  QString msg = QString("%1: " HTML_STYLE_OPEN(color:orange) "%2" HTML_STYLE_CLOSE())
      .arg(tr("Audio Delay Time"))
      .arg(t.toString("m:ss"));
  logger_->showMessageOnce(msg);
}

void
EventLogger::logCacheCleared()
{ logger_->showMessage(tr("offline cache removed")); }

void
EventLogger::logTrackedWindowDestroyed()
{ logger_->showMessage(tr("tracked window closed")); }

void
EventLogger::logServerAgentConnectionError()
{ logger_->warn(tr("not connected to the Internet")); }

void
EventLogger::logServerAgentError404()
{ logger_->warn(tr("got error 404 from remote server")); }

void
EventLogger::logServerAgentServerError()
{ logger_->warn(tr("got error reply from remote server")); }

void
EventLogger::logServerAgentUnknownError()
{ logger_->warn(tr("unknown SOAP error")); }

void
EventLogger::logClientAgentAuthorized()
{ logger_->showMessage(tr("server authorization succeeded")); }

void
EventLogger::logClientAgentDeauthorized()
{ logger_->showMessage(tr("server deauthorized")); }

void
EventLogger::logClientAgentAuthorizationError()
{ logger_->warn(tr("failed to authorize server, mismatched public key")); }

void
EventLogger::logTextEncodingChanged(const QString &enc)
{ logger_->showMessage(tr("text encoding") + ": " HTML_STYLE_OPEN(color:orange) + enc + HTML_STYLE_CLOSE()); }

void
EventLogger::logAspectRatioChanged(const QString &ratio)
{
  if (ratio.isEmpty())
    logger_->showMessage(tr("video aspect ratio set to default"));
  else
    logger_->showMessage(tr("video aspect ratio") + ": "
        HTML_STYLE_OPEN(color:orange) + ratio + HTML_STYLE_CLOSE());
}

void
EventLogger::logAeroEnabledChanged(bool t)
{
  if (AcUi::globalInstance()->isAeroEnabled())
    logger_->notify(tr("Aero is enabled, please restart the program"));
  else if (t)
    logger_->warn(tr("failed to enable Aero"));
  else
    logger_->notify(tr("Aero is disabled, please restart the program"));
}

void
EventLogger::logMenuEnabledChanged(bool t)
{
  if (t)
    logger_->notify(tr("Menu theme is enabled, please restart the program"));
  else
    logger_->notify(tr("Menu theme is disabled, please restart the program"));
}

void
EventLogger::logContrastChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    logger_->showMessageOnce(tr("Reset Contrast"));
  else
    logger_->showMessageOnce(tr("Contrast") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logBrightnessChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    logger_->showMessageOnce(tr("Rest Brightness"));
  else
    logger_->showMessageOnce(tr("Brightness") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logHueChanged(int value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (value)
    logger_->showMessageOnce(tr("Hue") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
  else
    logger_->showMessageOnce(tr("Reset Hue"));
}

void
EventLogger::logSaturationChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    logger_->showMessageOnce(tr("Reset Saturation"));
  else
    logger_->showMessageOnce(tr("Saturation") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logGammaChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    logger_->showMessageOnce(tr("Reset Gamma"));
  else
    logger_->showMessageOnce(tr("Gamma") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logAnnotationScaleChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    logger_->showMessageOnce(tr("Reset Scale"));
  else
    logger_->showMessageOnce(tr("Scale") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logAnnotationSpeedFactorChanged(int value)
{
  enum { DefaultValue = 100 };
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (value == DefaultValue)
    logger_->showMessageOnce(tr("Reset Moving Speed"));
  else {
    QString t = QString("X%1").arg(QString::number(value/qreal(DefaultValue), 'f', 2));
    logger_->showMessageOnce(tr("Moving Speed") + ": "
        HTML_STYLE_OPEN(color:orange) + t + HTML_STYLE_CLOSE());
  }
}

void
EventLogger::logAnnotationRotationChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value + 1, 1.0))
    logger_->showMessageOnce(tr("Reset Rotation"));
  else
    logger_->showMessageOnce(tr("Rotate") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logAnnotationOffsetChanged(int value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (value)
    logger_->showMessageOnce(tr("Annotation Delay Time") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE() +
        tr(" sec"));
  else
    logger_->showMessageOnce(tr("Reset Annotation Time"));
}

void
EventLogger::logAnnotationCountLimitedChanged(bool value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (value)
    logger_->showMessage(tr("limit annotations bandwidth"));
  else
    logger_->showMessage(tr("display all annotations"));
}

void
EventLogger::logAnnotationSkipped()
{
  enum { interval = 2000 }; // 2 seconds
  static qint64 time = 0;
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (now <= time + interval) {
    time = now;
    return;
  }
  time = now;
  logger_->notify(tr("so many annotations, skipped"));
}

void
EventLogger::logPauseHoveredAnnotations(bool t)
{
  if (t)
    logger_->showMessage(tr("capture hovered annotations"));
}

void
EventLogger::logResumeHoveredAnnotations(bool t)
{
  if (t)
    logger_->showMessage(tr("release hovered annotations"));
}

void
EventLogger::logRemoveHoveredAnnotations(bool t)
{
  Q_UNUSED(t);
  //if (t && !hub_->isFullScreenWindowMode())
  //  logger_->showMessage(tr("remove hovered annotations"));
}

void
EventLogger::logExpelNearbyAnnotations(bool t)
{
  Q_UNUSED(t);
  //if (t && !hub_->isFullScreenWindowMode())
  //  logger_->showMessage(tr("scatter nearby annotations"));
}

void
EventLogger::logAttractNearbyAnnotations(bool t)
{
  Q_UNUSED(t);
  //if (t && !hub_->isFullScreenWindowMode())
  //  logger_->showMessage(tr("collect nearby annotations"));
}

void
EventLogger::logInternetConnectionChanged(bool t)
{
  if (t)
    logger_->showMessage(tr("connected to the Internet"));
  else
    logger_->showMessage(tr("disconnected from the Internet"));
}

void
EventLogger::logSelectedUserIds(const QList<qint64> &uids)
{
  enum { timeout = 500 }; // msecs
  static qint64 ts = 0;
  qint64 count = uids.size();
  if (count) {
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now > ts + timeout)
      logger_->showMessageOnce(tr("Found %1 Users").arg(
        HTML_STYLE_OPEN(color:orange) + QString::number(count) + HTML_STYLE_CLOSE()
      ));
    ts = now;
  }
}

void
EventLogger::logPreferMotionlessAnnotationChanged(bool t)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (t)
    logger_->showMessage(tr("prefer motionless annotations"));
  else
    logger_->showMessage(tr("prefer floating annotations"));
}

void
EventLogger::logPreferLocalDatabaseChanged(bool t)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (t)
    logger_->showMessage(tr("prefer offline annotations over online ones"));
  else
    logger_->showMessage(tr("prefer online annotations over offline ones"));
}

void
EventLogger::logCanvasEnabled(bool t)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }
  if (t)
    logger_->showMessage(tr("show annotation analytics while playing"));
  else
    logger_->showMessage(tr("hide annotation analytics while playing"));
}

void
EventLogger::logFileSaved(const QString &fileName)
{
  logger_->showMessage(tr("file saved") + ": " + fileName);
  QApplication::beep();
}

// EOF
