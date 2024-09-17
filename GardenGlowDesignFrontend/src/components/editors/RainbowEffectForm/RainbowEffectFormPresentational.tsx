// RainbowEffectForm.tsx
import React from 'react';
import {Field} from 'formik';
import {EffectForm, labelColumns, fieldColumns} from '../EffectForms';
import {Effect} from '../../serialization/Effect';
import Grid from "@mui/material/Grid";

interface RainbowEffectFormProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete?: (effectId: number) => void;
    onClose?: () => void;
}

export const RainbowEffectFormPresentational:
    React.FC<RainbowEffectFormProps> = (props) => {
    return (
        <EffectForm {...props}>
            <Grid item xs={labelColumns}>
                <label htmlFor="rainbowSpeed">Rainbow Speed</label>
            </Grid>
            {/* TODO: Add fields for color */}
            <Grid item xs={fieldColumns}>
                <Field id="speed" name="speed" type="number"/>
            </Grid>
        </EffectForm>
    );
};
