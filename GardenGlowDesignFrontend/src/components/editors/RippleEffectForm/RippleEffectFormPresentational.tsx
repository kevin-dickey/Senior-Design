import React from 'react';
import {Field} from 'formik';
import {EffectForm} from '../EffectForms';
import {Effect} from '../../serialization/Effect';
import Grid from "@mui/material/Grid";

interface RippleEffectFormProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete?: (effectId: number) => void;
}

export const RippleEffectFormPresentational:
    React.FC<RippleEffectFormProps> = (props) => {
    const labelColumns = 7;
    const fieldColumns = 12 - labelColumns;
    return (
        <EffectForm {...props}>
            <Grid item xs={labelColumns}>
                <label htmlFor="rippleSpeed">Ripple Speed</label>
            </Grid>
            <Grid item xs={fieldColumns}>
                <Field id="speed" name="speed" type="number" placeholder="Enter value (1-5)" />
            </Grid>
        </EffectForm>
    );
};
