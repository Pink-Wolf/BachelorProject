import { Declaration } from "../CodeComponents";
import FormattedText from "../FormattedText";

export default function AliasViewer({ data }) {
	return (
		<section>
			<h1 id="Declaration">
				<Declaration>{data.declaration}</Declaration>
			</h1>
			<p id="Description">
				<FormattedText>{data.detailedDescription}</FormattedText>
			</p>
		</section>
	)
}