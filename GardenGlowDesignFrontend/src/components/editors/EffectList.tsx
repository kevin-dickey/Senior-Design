import React from "react";
import {Effect} from "../serialization/Effect";

interface EffectListProps {
    effects: Effect[];
    onEffectSelected: any;
    // This should be the set function from useState in Configuration.tsx
}

export const EffectList: React.FC<EffectListProps> =
    ({effects, onEffectSelected}) => {
    return (
        <ul>
            {effects.map((effect, index) => (
                <li key={index} onClick={() => onEffectSelected(effect.id)}>
                    {effect.name}
                </li>
            ))}
        </ul>
    );
}