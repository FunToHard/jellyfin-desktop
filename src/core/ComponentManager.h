#ifndef __COMPONENT_MANAGER_H__
#define __COMPONENT_MANAGER_H__

#include <QObject>
#include <QMap>
#include <QQmlContext>
#include <QQmlPropertyMap>
#include <QWebChannel>

#include "utils/Utils.h"

class ComponentBase : public QObject
{
public:
  explicit ComponentBase(QObject* parent = nullptr) : QObject(parent) { }
  
  virtual bool componentInitialize() = 0;
  virtual const char* componentName() = 0;
  virtual bool componentExport() = 0;

  // executed after ALL components are initialized
  virtual void componentPostInitialize() { }

  // executed before application shutdown
  virtual void componentShutdown() { }
};

class ComponentManager : public QObject
{
  Q_OBJECT
  DEFINE_SINGLETON(ComponentManager);

public:
  void initialize();
  void shutdown();
  inline QQmlPropertyMap &getQmlPropertyMap() { return m_qmlProperyMap; }
  void setWebChannel(QWebChannel* webChannel);

private:
  ComponentManager();
  void registerComponent(ComponentBase* comp);

  QMap<QString, ComponentBase*> m_components;
  QList<ComponentBase*> m_registeredOrder;
  QQmlPropertyMap m_qmlProperyMap;
};

#endif
