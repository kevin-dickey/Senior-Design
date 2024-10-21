import { Show } from "../components/serialization/Show";
import {Effect, RainbowEffect, RippleEffect} from "../components/serialization/Effect";
import { Layout } from "../components/serialization/Layout";

export const deserializeShow = (data: any): Show => {
    const show = new Show(data.name, data.durationMs);

    // Create the effects array from the data. Use different constructors for effects based on
    // their 'type' parameter such as 'RainbowEffect' or 'RippleEffect'.
    const effects = data.effects.map((effectData: any) => {
        switch (effectData.type) {
            case 'RainbowEffect':
                return RainbowEffect.fromJSON(effectData);
            case 'RippleEffect':
                return RippleEffect.fromJSON(effectData);
            case 'Effect':
                return Effect.fromJSON(effectData);
            default:
                throw new Error(`Unknown effect type: ${effectData.type}`);
        }}
    );

    show.setEffects(effects);
    show.layouts = data.layouts.map((layoutData: any) => Layout.fromJSON(layoutData));
    return show;
};