import { Show } from "../components/serialization/Show";
import { Effect } from "../components/serialization/Effect";
import { Layout } from "../components/serialization/Layout";

export const deserializeShow = (data: any): Show => {
    const show = new Show(data.name, data.duration);
    show.setEffects(data.effects.map((effectData: any) => Effect.fromJSON(effectData)));
    show.layouts = data.layouts.map((layoutData: any) => Layout.fromJSON(layoutData));
    return show;
};