#ifndef K_QP_H_
#define K_QP_H_

namespace K {
  static json defQP {
    {"widthPing",                     2},
    {"widthPingPercentage",           decimal_cast<2>("0.25").getAsDouble()},
    {"widthPong",                     2},
    {"widthPongPercentage",           decimal_cast<2>("0.25").getAsDouble()},
    {"widthPercentage",               false},
    {"bestWidth",                     true},
    {"buySize",                       decimal_cast<2>("0.02").getAsDouble()},
    {"buySizePercentage",             7},
    {"buySizeMax",                    false},
    {"sellSize",                      decimal_cast<2>("0.01").getAsDouble()},
    {"sellSizePercentage",            7},
    {"sellSizeMax",                   false},
    {"pingAt",                        (int)mPingAt::BothSides},
    {"pongAt",                        (int)mPongAt::ShortPingFair},
    {"mode",                          (int)mQuotingMode::AK47},
    {"bullets",                       2},
    {"range",                         decimal_cast<1>("0.5").getAsDouble()},
    {"fvModel",                       (int)mFairValueModel::BBO},
    {"targetBasePosition",            1},
    {"targetBasePositionPercentage",  50},
    {"positionDivergence",            decimal_cast<1>("0.9").getAsDouble()},
    {"positionDivergencePercentage",  21},
    {"percentageValues",              false},
    {"autoPositionMode",              (int)mAutoPositionMode::EWMA_LS},
    {"aggressivePositionRebalancing", (int)mAPR::Off},
    {"superTrades",                   (int)mSOP::Off},
    {"tradesPerMinute",               decimal_cast<1>("0.9").getAsDouble()},
    {"tradeRateSeconds",              69},
    {"quotingEwmaProtection",         true},
    {"quotingEwmaProtectionPeridos",  200},
    {"quotingStdevProtection",        (int)mSTDEV::Off},
    {"quotingStdevBollingerBands",    false},
    {"quotingStdevProtectionFactor",  decimal_cast<1>("1.0").getAsDouble()},
    {"quotingStdevProtectionPeriods", 1200},
    {"ewmaSensiblityPercentage",      decimal_cast<1>("0.5").getAsDouble()},
    {"longEwmaPeridos",               200},
    {"mediumEwmaPeridos",             100},
    {"shortEwmaPeridos",              50},
    {"aprMultiplier",                 2},
    {"sopWidthMultiplier",            2},
    {"delayAPI",                      0},
    {"cancelOrdersAuto",              false},
    {"cleanPongsAuto",                0},
    {"profitHourInterval",            decimal_cast<1>("0.5").getAsDouble()},
    {"audio",                         false},
    {"delayUI",                       7}
  };
  class QP {
    public:
      static void main(Local<Object> exports) {
        Isolate* isolate = exports->GetIsolate();
        for (json::iterator it = defQP.begin(); it != defQP.end(); ++it) {
          string k = CF::cfString(it.key(), false);
          if (k != "") defQP[it.key()] = k;
        }
        qpRepo = defQP;
        json qpa = DB::load(uiTXT::QuotingParametersChange);
        if (qpa.size())
          for (json::iterator it = qpa["/0"_json_pointer].begin(); it != qpa["/0"_json_pointer].end(); ++it)
            qpRepo[it.key()] = it.value();
        cleanBool();
        UI::uiSnap(uiTXT::QuotingParametersChange, &onSnap);
        UI::uiHand(uiTXT::QuotingParametersChange, &onHand);
        EV::evUp("QuotingParameters", v8qp(qpRepo));
        NODE_SET_METHOD(exports, "qpRepo", QP::_qpRepo);
      }
      static void _qpRepo(const FunctionCallbackInfo<Value> &args) {
        args.GetReturnValue().Set(v8qp(qpRepo));
      };
      static json onSnap(json z) {
        return { qpRepo };
      };
      static json onHand(json k) {
        if (k["buySize"].get<double>() > 0
          && k["sellSize"].get<double>() > 0
          && k["buySizePercentage"].get<double>() > 0
          && k["sellSizePercentage"].get<double>() > 0
          && k["widthPing"].get<double>() > 0
          && k["widthPong"].get<double>() > 0
          && k["widthPingPercentage"].get<double>() > 0
          && k["widthPongPercentage"].get<double>() > 0
        ) {
          if ((mQuotingMode)k["mode"].get<int>() == mQuotingMode::Depth)
            k["widthPercentage"] = false;
          qpRepo = k;
          cleanBool();
          Local<Object> o = v8qp(k);
          DB::insert(uiTXT::QuotingParametersChange, o);
          EV::evUp("QuotingParameters", o);
        }
        JSON Json;
        UI::uiSend(uiTXT::QuotingParametersChange, k);
        return {};
      };
      static Local<Object> v8qp(json k) {
        Isolate* isolate = Isolate::GetCurrent();
        Local<Object> o = Object::New(isolate);
        for (json::iterator it = k.begin(); it != k.end(); ++it)
          if (it.value().is_number()) o->Set(FN::v8S(it.key()), Number::New(isolate, it.value()));
          else if (it.value().is_boolean()) o->Set(FN::v8S(it.key()), Boolean::New(isolate, it.value()));
        return o;
      };
      static void cleanBool() {
        if (qpRepo["widthPercentage"].is_number())
          qpRepo["widthPercentage"] = qpRepo["widthPercentage"].get<int>() != 0;
        if (qpRepo["quotingStdevBollingerBands"].is_number())
          qpRepo["quotingStdevBollingerBands"] = qpRepo["quotingStdevBollingerBands"].get<int>() != 0;
        if (qpRepo["quotingEwmaProtection"].is_number())
          qpRepo["quotingEwmaProtection"] = qpRepo["quotingEwmaProtection"].get<int>() != 0;
        if (qpRepo["percentageValues"].is_number())
          qpRepo["percentageValues"] = qpRepo["percentageValues"].get<int>() != 0;
        if (qpRepo["widthPercentage"].is_number())
          qpRepo["widthPercentage"] = qpRepo["widthPercentage"].get<int>() != 0;
        if (qpRepo["bestWidth"].is_number())
          qpRepo["bestWidth"] = qpRepo["bestWidth"].get<int>() != 0;
        if (qpRepo["buySizeMax"].is_number())
          qpRepo["buySizeMax"] = qpRepo["buySizeMax"].get<int>() != 0;
        if (qpRepo["sellSizeMax"].is_number())
          qpRepo["sellSizeMax"] = qpRepo["sellSizeMax"].get<int>() != 0;
        if (qpRepo["cancelOrdersAuto"].is_number())
          qpRepo["cancelOrdersAuto"] = qpRepo["cancelOrdersAuto"].get<int>() != 0;
        if (qpRepo["audio"].is_number())
          qpRepo["audio"] = qpRepo["audio"].get<int>() != 0;
      };
  };
}

#endif
