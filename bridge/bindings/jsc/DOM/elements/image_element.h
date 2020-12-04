/*
 * Copyright (C) 2020 Alibaba Inc. All rights reserved.
 * Author: Kraken Team.
 */

#ifndef KRAKENBRIDGE_IMAGE_ELEMENT_H
#define KRAKENBRIDGE_IMAGE_ELEMENT_H

#include "bindings/jsc/DOM/element.h"
#include "bindings/jsc/js_context.h"

namespace kraken::binding::jsc {

struct NativeImageElement;

void bindImageElement(std::unique_ptr<JSContext> &context);

class JSImageElement : public JSElement {
public:
  static std::unordered_map<JSContext *, JSImageElement *> instanceMap;
  static JSImageElement *instance(JSContext *context);
  JSObjectRef instanceConstructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount,
                                  const JSValueRef *arguments, JSValueRef *exception) override;

  class ImageElementInstance : public ElementInstance {
  public:
    enum class ImageProperty {
      kWidth, kHeight,
      kSrc,
      kLoading
    };

    static std::vector<JSStringRef> &getImageElementPropertyNames();
    static const std::unordered_map<std::string, ImageProperty> &getImageElementPropertyMap();

    ImageElementInstance() = delete;
    ~ImageElementInstance();
    explicit ImageElementInstance(JSImageElement *JSImageElement);
    JSValueRef getProperty(std::string &name, JSValueRef *exception) override;
    void setProperty(std::string &name, JSValueRef value, JSValueRef *exception) override;
    void getPropertyNames(JSPropertyNameAccumulatorRef accumulator) override;

    NativeImageElement *nativeImageElement;

  private:
    JSStringHolder m_src{context, ""};
    JSStringHolder m_loading{context, ""};
  };
protected:
  JSImageElement() = delete;
  explicit JSImageElement(JSContext *context);
  ~JSImageElement();
};

using GetImageWidth = double(*)(NativeImageElement *nativeImageElement);
using GetImageHeight = double(*)(NativeImageElement *nativeImageElement);

struct NativeImageElement {
  NativeImageElement() = delete;
  explicit NativeImageElement(NativeElement *nativeElement) : nativeElement(nativeElement){};

  NativeElement *nativeElement;

  GetImageWidth getImageWidth;
  GetImageHeight getImageHeight;
};

} // namespace kraken::binding::jsc

#endif // KRAKENBRIDGE_IMAGE_ELEMENT_H
