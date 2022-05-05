bool titleValid, headingValid, iconValid, settingsValid, isLeeched;

void setText(JNIEnv *env, jobject obj, const char* text){
    //https://stackoverflow.com/a/33627640/3763113
    //A little JNI calls here. You really really need a great knowledge if you want to play with JNI stuff
    //Html.fromHtml("");
    jclass html = (*env).FindClass(OBFUSCATE("android/text/Html"));
    jmethodID fromHtml = (*env).GetStaticMethodID(html, OBFUSCATE("fromHtml"), OBFUSCATE("(Ljava/lang/String;)Landroid/text/Spanned;"));

    //setText("");
    jclass textView = (*env).FindClass(OBFUSCATE("android/widget/TextView"));
    jmethodID setText = (*env).GetMethodID(textView, OBFUSCATE("setText"), OBFUSCATE("(Ljava/lang/CharSequence;)V"));

    //Java string
    jstring jstr = (*env).NewStringUTF(text);
    (*env).CallVoidMethod(obj, setText,  (*env).CallStaticObjectMethod(html, fromHtml, jstr));
}


void setTitleText(JNIEnv *env, jobject thiz, jobject obj) {
    setText(env, obj, OBFUSCATE("<b>IOSGODS</b>"));

    titleValid = true;
}

void setHeadingText(JNIEnv *env, jobject thiz, jobject obj) {
    setText(env, obj, OBFUSCATE("<b><marquee><p style=\"font-size:30\">"
                                      "<p style=\"color:green;\">Modded by xxbatman9898</p> | "
                                      "GrimSoul 3.6.0 Menu</p>"
                                      "</marquee></b>"));

    headingValid = true;
}

jstring Icon(JNIEnv *env, jobject thiz) {
    iconValid = true;

    //Use https://www.base64encode.org/ to encode your image to base64
    return env->NewStringUTF(
            OBFUSCATE("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"));
}

jstring IconWebViewData(JNIEnv *env, jobject thiz) {
    iconValid = true;
    //WebView support GIF animation. Upload your image or GIF on imgur.com or other sites

    // From internet (Requires android.permission.INTERNET)
    // return env->NewStringUTF(OBFUSCATE("https://i.imgur.com/SujJ85j.gif"));

    // Base64 html:
    // return env->NewStringUTF("data:image/png;base64, <encoded base64 here>");

    // To disable it, return NULL. It will use normal image above:
    // return NULL

    //return env->NewStringUTF(OBFUSCATE_KEY("https://i.imgur.com/SujJ85j.gif", 'u'));
    return NULL;
}

jobjectArray settingsList(JNIEnv *env, jobject activityObject) {
    jobjectArray ret;

    const char *features[] = {
            OBFUSCATE("Category_Settings"),
            OBFUSCATE("-1_Toggle_Save feature preferences"), //-1 is checked on Preferences.java
            OBFUSCATE("-3_Toggle_Auto size vertically"),
            OBFUSCATE("Category_Logcat"),
            OBFUSCATE("RichTextView_Save logcat if a bug occured and sent it to the modder. Clear logcat and reproduce bug again if the log file is too large"),
            OBFUSCATE("RichTextView_<small>Saving logcat does not need file permission. Logcat location:"
                            "<br/>Android 11: /storage/emulated/0/Documents/"
                            "<br/>Android 10 and below: /storage/emulated/0/Android/data/(package name)/files/Mod Menu</small>"),
            OBFUSCATE("-4_Button_Save logcat to file"),
            OBFUSCATE("-5_Button_Clear logcat"),
            OBFUSCATE("Category_Menu"),
            OBFUSCATE("-6_Button_<font color='red'>Close settings</font>"),
    };

    int Total_Feature = (sizeof features /
                         sizeof features[0]); //Now you dont have to manually update the number everytime;
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));
    int i;
    for (i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    settingsValid = true;

    return (ret);
}


