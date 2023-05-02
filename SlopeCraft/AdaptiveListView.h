#ifndef SLOPECRAFT_SLOPECRAFT_ADAPTIVELISTVIEW_H
#define SLOPECRAFT_SLOPECRAFT_ADAPTIVELISTVIEW_H
#include <QListView>
#include <QResizeEvent>

class AdaptiveListView : public QListView {
 public:
  explicit AdaptiveListView(QWidget* parent = nullptr);
  ~AdaptiveListView();

  void resizeEvent(QResizeEvent* event) override;
};

#endif  // SLOPECRAFT_SLOPECRAFT_ADAPTIVELISTVIEW_H
