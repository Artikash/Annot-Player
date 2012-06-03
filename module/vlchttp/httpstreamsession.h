#ifndef HTTPSTREAMSESSION_H
#define HTTPSTREAMSESSION_H

// httpstreamsession.h
// 3/14/2012

#include "module/vlchttp/httpsession.h"
#include "module/stream/inputstream.h"
#include "module/stream/fifostream.h"
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QList>

//QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class RemoteStream;
class FlvMerge;

class HttpStreamSession : public VlcHttpSession
{
  Q_OBJECT
  Q_DISABLE_COPY(HttpStreamSession)
  typedef HttpStreamSession Self;
  typedef VlcHttpSession Base;

  QList<QUrl> urls_;
  //QNetworkAccessManager *nam_;
  qint64 duration_;
  InputStreamList ins_;
  FifoStream *fifo_;
  FlvMerge *merger_;

  QMutex m_;
  QWaitCondition readyCond_, readyReadCond_, stoppedCond_;
  bool ready_;

  QString fileName_;

  StoppableTask *progressTask_;

  //struct Progress
  //{
  //  qint64 receivedBytes, totalBytes;
  //  explicit Progress(qint64 r = 0, qint64 t = 0) : receivedBytes(r), totalBytes(t) { }
  //};
  //QHash<long, Progress> progress_;

public:
  HttpStreamSession(const QList<QUrl> &urls, qint64 duration, QObject *parent = 0)
    : Base(parent), urls_(urls), duration_(duration), fifo_(0), merger_(0),
      ready_(false), progressTask_(0) { }

  ~HttpStreamSession();

signals:
  void stopped();

  // - Properties -
public:
  const QList<QUrl> &urls() const { return urls_; }
  virtual QString contentType() const; ///< \override

  virtual qint64 read(char *data, qint64 maxSize)  ///< \override
  { return fifo_ ? fifo_->read(data, maxSize) : 0; }

  virtual bool seek(qint64 pos) ///< \override
  { return fifo_ ? fifo_->seek(pos) : false; } ///< \override

  virtual qint64 size() const { return fifo_ ? fifo_->size() : 0; } ///< \override
  virtual qint64 pos() const { return fifo_ ? fifo_->pos() : 0; } ///< \override
  virtual qint64 availableSize() const { return fifo_ ? fifo_->availableSize() : 0; } ///< \override

  virtual qint64 duration() const { return duration_; } ///< \override
  virtual qint64 availableDuration() const; ///< \override

  qint64 receivedSize() const;

public slots:
  void setUrls(const QList<QUrl> &urls) { urls_ = urls; }
  void setDuration(qint64 duration) { duration_ = duration; }

  virtual void run(); ///< \override
  virtual void stop(); ///< \override

  virtual void waitForReady(); ///< \override
  virtual void waitForStopped(); ///< \override

  void updateProgress() { emit progress(receivedSize(), size()); }

protected slots:
  virtual void save(); ///< \override

  void updateSize();
  void updateFileName();
  void finish();

  void wakeAllStopped() { stoppedCond_.wakeAll(); }
  void wakeAllReady() { readyCond_.wakeAll(); }
  void wakeAllReadyRead() { readyReadCond_.wakeAll(); }

  //void updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id);
};

#endif // HTTPSTREAMSESSION_H