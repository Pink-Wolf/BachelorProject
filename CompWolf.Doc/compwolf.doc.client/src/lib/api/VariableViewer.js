import { Declaration, FormattedText } from "./CodeComponents";
import BaseEntityViewer from "./BaseEntityViewer";
import { Fragment } from "react";

export default function VariableViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<BaseEntityViewer data={data} top={
			<Fragment>
				<p id="Declaration">
					<Declaration>{data.declaration}</Declaration>
				</p>
				<p id="Description">
					<FormattedText>{data.detailedDescription}</FormattedText>
				</p>
			</Fragment>
		}>
		</BaseEntityViewer>
	)
}