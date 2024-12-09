// RainbowEffectForm.tsx
import React from 'react';
import {ErrorMessage, Field} from 'formik';
import {EffectForm, labelColumns, fieldColumns} from '../EffectForms';
import {Effect} from '../../serialization/Effect';
import Grid from "@mui/material/Grid";

interface RainbowEffectFormProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete?: (effectId: number) => void;
}

export const RainbowEffectFormPresentational:
    React.FC<RainbowEffectFormProps> = (props) => {
    
        return (
        <EffectForm {...props}>
            <Grid item xs={labelColumns}>
                <label htmlFor="rainbowSpeed">Speed</label>
            </Grid>
            {/* TODO: Add fields for color */}
            <Grid item xs={fieldColumns}>
                <Field id="speed" 
                name="speed" 
                type="number" 
                placeholder="(1-5)"
                min="1" 
                max="5"
                step="1" 
                validate={(value : string | number | undefined) => {
                    if (!value) return "Speed is required"; 
                    if (typeof value === "number" && value < 1) return "Speed must be at least 1";
                    if (!Number.isInteger(Number(value))) return "Speed must be an integer";
                    if (typeof value === "number" && value > 5) return "Speed must be less than 6";
                    return undefined; 
                }}
            />
            <ErrorMessage name="speed" component="div" />
            </Grid>
        </EffectForm>
    );
};
